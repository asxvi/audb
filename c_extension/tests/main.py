from numerize import numerize
import random
import time
import os
import psycopg2, psycopg2.extras
import numpy as np
import pandas as pd
from dataclasses import dataclass
import matplotlib.pyplot as plt

from cliUtility import *
from DataTypes import *

@dataclass
class ExperimentGroup:
    '''Container class that stores groups of single IV experiments (Dicts of ExperimentSettings)'''
    name: str
    independent_variable: str
    experiments: dict = None
    
    def __post_init__(self):
        if self.experiments is None:
            self.experiments = {}

@dataclass
class ExperimentSettings:
    '''
        Class contains the modifiable settings of a test
        if num_intervals is used, num_intervals_range shouldn't be used
        if gap_size is used, gap_size_range shouldn't be used
    '''
    name: str                                   # required 
    data_type: DataType                         # always Set or Range
    curr_trial: int = 0                         # keep track locally 
    experiment_id: str = None                   # unique string name that identifies specific experiment
    num_trials: int = 1                         # always fixed
    dataset_size: int = 100                     # always fixed
    uncertain_ratio: float = 0.00               # uncertainty ratio is split 50% in data, 50% in multiplicity columns. Uncert in data == NULL, uncert in mult = [0,N]
    interval_size_range: tuple = (1, 1000)      # the size of each interval
    mult_size_range: tuple = (1,5)              # required 
    independent_variable: str = None            # flag for what var we test. Used internally
    start_interval_range: tuple = (interval_size_range[0], interval_size_range[1])   
    reduce_triggerSz_sizeLim: tuple = (10,5)    # test this. need to figure out how to encode different techniques.
    
    # use these value if not None, otherwise use tuple if not None, both none = error
    interval_width: int = None
    interval_width_range: tuple = None
    num_intervals: int = None       
    gap_size: int = None
    num_intervals_range: tuple = None
    gap_size_range: tuple = None    
    
    mode: str = None                # NOT USED YET what modes of test suite to execute
    save_ddl:bool = False           # store ddl code to make tables 
    save_csv: bool = True           # store csv with statistics and results of test

    iv_map = {
        "dataset_size": "n",
        "uncertain_ratio": "unc",
        "interval_size_range": "isr",
        "mult_size_range": "msr",
        "num_intervals": "ni_nir",
        "num_intervals_range": "ni_nir",
        "gap_size": "gs_gsr",
        "gap_size_range": "gs_gsr",
        "reduce_triggerSz_sizeLim": "red_sz"
    }

    def asdict(self):
        dt = 'range' if self.data_type == DataType.RANGE else 'set'
        return {
            'name': self.name,
            'data_type': dt,
            'curr_trial': self.curr_trial,
            'experiment_id': self.experiment_id,
            'num_trials': self.num_trials,
            'dataset_size': self.dataset_size,
            'uncertain_ratio': self.uncertain_ratio,
            "interval_size_range": self.interval_size_range,
            'mult_size_range': self.mult_size_range,
            'num_intervals': self.num_intervals,
            'gap_size': self.gap_size,
            'num_intervals_range': self.num_intervals_range,
            'gap_size_range': self.gap_size_range
        }
    
class ExperimentRunner:
    '''
        ExperimentRunner runs entire or parts of a test (gen_data, insert_db)
    '''
    def __init__(self, db_config, seed):
        self.db_config = db_config
        self.results = []
        self.master_seed = seed
        self.trial_seed = None
        self.resultFilepath: str = None
        self.name = None
        self.groupName = None

    DATA_TYPE_CONFIG = {
        DataType.RANGE: {
            "combine_sum": "combine_range_mult_sum",
            "combine_min": "combine_range_mult_min",
            "combine_max": "combine_range_mult_max",
        },
        DataType.SET: {
            "combine_sum": "combine_set_mult_sum",
            "combine_min": "combine_set_mult_min",
            "combine_max": "combine_set_mult_max",
        },
    }

    def run_experiment(self, experiment: ExperimentSettings) -> list:
        '''an experiement has N trials. gen data for each trial, run queries//benchmark results, and append to results'''    
        # generate data for each trial. Insert ddl to file optinally. After inserting to DB, run tests
        experiment_results = []
        
        for trial in range(experiment.num_trials):
            # create a trial seed dependent on the master seed, and specific trial number
            experiment.curr_trial = trial+1
            self.trial_seed = (self.master_seed + experiment.curr_trial) % (2**32)
            np.random.seed(self.trial_seed)
            experiment.experiment_id = self.__generate_name(experiment)
            
            # get randomly generated data for curr seed
            db_data_format, file_data_format = self.generate_data(experiment)

            # save in ddl compatible format. Insert into DB regardless... need to run tests.
            # DOES NOT WORK properly
            if experiment.save_ddl:
                self.__save_ddl_file(experiment, file_data_format)
            
            self.insert_data_db(experiment, db_data_format)

            # run queries and benchmark
            trial_results = self.run_queries(experiment)
            experiment_results.append(trial_results)

        aggregated_results = self.__calc_aggregate_results(experiment, experiment_results)
        self.results.append(aggregated_results)

        return experiment_results
        
    def generate_data(self, experiment :ExperimentSettings):
        '''
            Generates pseudorandom data based on user specified experiment settings. 
            * NOTE Specfic data serialization for different formats (i.e file and db ddl differs)
        '''
        db_formatted_rows = []
        file_formatted_rows = []
        
        for i in range(experiment.dataset_size):
            if experiment.data_type == DataType.RANGE:
                obj = self.__generate_range(experiment)
                val = str(obj) if not obj.isNone else None
            elif experiment.data_type == DataType.SET:
                obj = self.__generate_set(experiment)
                val = str(obj) if (obj.rset and not getattr(obj, 'isNone', False)) else None

            mult_obj = self.__generate_mult(experiment)
            mult = str(mult_obj)
            db_formatted_rows.append((val, mult))
            
            # save in ddl preffered format if requested
            if experiment.save_ddl:
                val = obj.str_ddl()
                mult = mult_obj.str_ddl()
                file_formatted_rows.append((val, mult))
                
        return db_formatted_rows, file_formatted_rows

    def run_queries(self, experiment: ExperimentSettings):
        '''Run same aggregation tests on both DataTypes.'''
        
        results = {
            'row_count' : 0,
            'min_time' : None,
            'min_result' : None,
            'max_time' : None,
            'max_result' : None,
            'sum_time' : None,
            'sum_result' : None,
        }
        table = experiment.experiment_id
        config = self.DATA_TYPE_CONFIG[experiment.data_type]

        try:
            with self.connect_db() as conn:
                with conn.cursor() as cur:
                    cur.execute(f"SELECT COUNT(*) FROM {table};")
                    results['row_count'] = cur.fetchone()[0]
                    results['sum_time'] = self.run_aggregate(cur, table, 'SUM', config['combine_sum'], experiment.reduce_triggerSz_sizeLim[0], experiment.reduce_triggerSz_sizeLim[1])
                    results['min_time'] = self.run_aggregate(cur, table, 'MIN', config['combine_min'])
                    results['max_time'] = self.run_aggregate(cur, table, 'MAX', config['combine_max'])

        except Exception as e:
            print(f"Error running queries for {experiment.experiment_id}: {e}")
            exit(1)
        
        return results

    def run_aggregate(self, cur, table, agg_name, combine_func, *agg_params):
        '''General aggregate runner with no WHERE clause'''

        params_sql = ",".join(str(param) for param in agg_params)
        sql = f"""EXPLAIN (analyze, format json)
            SELECT {agg_name} ({combine_func}(val, mult) {',' if params_sql else ''}{params_sql})
            FROM {table};"""
        
        cur.execute(sql)
        results = cur.fetchone()[0]
        plan_root = results[0]
        plan = plan_root["Plan"]
        agg_time = plan["Actual Total Time"]
        return agg_time

    def insert_data_db(self, experiment: ExperimentSettings, data):
        '''Insert data into database specified in config file'''
        with self.connect_db() as conn:
            with conn.cursor() as cur:
                table_name = experiment.experiment_id
                cur.execute(f"DROP TABLE IF EXISTS {table_name};")

                if experiment.data_type == DataType.RANGE:
                    cur.execute(f"CREATE TABLE {table_name} (id INT GENERATED ALWAYS AS IDENTITY, val int4range, mult int4range);")                
                    template = "(%s::int4range, %s::int4range)"
                elif experiment.data_type == DataType.SET:
                    cur.execute(f"CREATE TABLE {table_name} (id INT GENERATED ALWAYS AS IDENTITY, val int4range[], mult int4range);")
                    template = "(%s::int4range[], %s::int4range)"
                
                sql = f"INSERT INTO {table_name} (val, mult) VALUES %s"
                psycopg2.extras.execute_values(cur, sql, data, template)
                conn.commit()
        
    def __generate_range(self, experiment:ExperimentSettings) -> RangeType:
        # uncertain ratio. maybe should account for half nulls, half mult 0
        if np.random.random() < experiment.uncertain_ratio * 0.5:  
            return RangeType(0, 0, True)

        lb = np.random.randint(*experiment.interval_size_range)
        ub = np.random.randint(lb+1, experiment.interval_size_range[1]+1)
        return RangeType(lb, ub)
    
    # def __generate_set(self, experiment:ExperimentSettings) -> RangeSetType:
    #     # if experiment.num_intervals is not None then use, otherwise if experiment.num_intervals_range then use. otherwise raise error
    #     if experiment.num_intervals is not None:
    #         num_intervals = experiment.num_intervals
    #     elif experiment.num_intervals_range is not None:
    #         num_intervals = np.random.randint(*experiment.num_intervals_range)
    #     else:
    #         raise ValueError("Either num_intervals or num_intervals_range must be specified")
        
    #     rset = []
    #     # FIXME account for gapsize. need to acocunt for (interval size range / (number interval * gap size))
    #     for i in range(num_intervals):    
    #         # uncertain ratio. maybe should account for half nulls, half mult 0
    #         if np.random.random() < experiment.uncertain_ratio * 0.5:  
    #             # rset.append(RangeType(0,0,True))
    #             continue
            
    #         lb = np.random.randint(*experiment.interval_size_range)
    #         ub = np.random.randint(lb+1, experiment.interval_size_range[1]+1)
            
    #         # if i < num_intervals - 1 and experiment.gap_size is not None or experiment.gap_size_range is not None:
    #         #     gap = experiment.gap_size if experiment.gap_size else np.random.randint(**experiment.gap_size_range)
                

    #         rset.append(RangeType(lb,ub,False))
        
    #     return RangeSetType(rset, cu=False)


    def __generate_set(self, experiment:ExperimentSettings) -> RangeSetType:
        
        # if experiment.num_intervals is not None then use, otherwise if experiment.num_intervals_range then use. otherwise raise error
        if experiment.num_intervals is not None:
            num_intervals = experiment.num_intervals
        elif experiment.num_intervals_range is not None:
            num_intervals = np.random.randint(*experiment.num_intervals_range)
        else:
            raise ValueError("Either num_intervals or num_intervals_range must be specified")
        
        # entire set is unknown
        if np.random.random() < experiment.uncertain_ratio * 0.5:  
            return RangeSetType([], cu=False)
        
        rset = []

        # set the first starting point
        if hasattr(experiment, 'start_interval_range') and experiment.start_interval_range is not None:
            start = np.random.randint(*experiment.start_interval_range)
        else:
            start = experiment.interval_size_range[0]

        # for each interval
        for i in range(num_intervals):    
            if np.random.random() < experiment.uncertain_ratio * 0.5:  
                continue
            
            # get the interval width
            if hasattr(experiment, 'interval_width') or hasattr(experiment, 'interval_width_range'):
                if experiment.interval_width is not None:
                    interval_width = experiment.interval_width
                elif experiment.interval_width_range is not None:
                    interval_width = np.random.randint(*experiment.interval_width_range)
                else:
                    raise ValueError("Either interval_width or interval_width_range must be specified")
                interval_end = start + interval_width
            else:
                raise ValueError("Either interval_width or interval_width_range must be specified")
                max_end = experiment.interval_size_range[1]
                interval_end = np.random(start+1, min(max_end + 1, start + 100))

            rset.append(RangeType(start, interval_end, False))

            # find next gap if not last
            if i < num_intervals -1:
                if experiment.gap_size is not None:
                    gap = experiment.gap_size
                elif experiment.gap_size_range is not None:
                    gap = np.random.randint(*experiment.gap_size_range)
                else:
                    gap = 0  
                
                start = interval_end + gap

        return RangeSetType(rset, cu=False)




    def __generate_mult(self, experiment:ExperimentSettings) -> RangeType:
        # uncertain ratio. maybe should account for half nulls, half mult 0
        if np.random.random() < experiment.uncertain_ratio * 0.5:  
            return RangeType(0, 0, True)
        
        lb = np.random.randint(*experiment.mult_size_range)
        ub = np.random.randint(lb+1, experiment.mult_size_range[1]+1)
        return RangeType(lb, ub)
    
    def __calc_aggregate_results(self, experiment: ExperimentSettings, trial_results: dict) -> dict:
        min_times = [r['min_time'] for r in trial_results if r['min_time'] is not None]
        max_times = [r['max_time'] for r in trial_results if r['max_time'] is not None]
        sum_times = [r['sum_time'] for r in trial_results if r['sum_time'] is not None]

        aggregated = {
            'uid' : self.__generate_name(experiment, True),
            'master_seed': self.master_seed,
            'data_type' : 'range' if experiment.data_type == DataType.RANGE else 'set',
            'num_trials': experiment.num_trials,
            'dataset_size' : experiment.dataset_size,
            'uncertain_ratio': experiment.uncertain_ratio,
            'interval_size_range':experiment.interval_size_range,
            'mult_size_range': experiment.mult_size_range,
            'num_intervals': experiment.num_intervals,
            'gap_size': experiment.gap_size,
            'num_intervals_range': experiment.num_intervals_range,
            'gap_size_range': experiment.gap_size_range,
            'reduce_triggerSz_sizeLim': experiment.reduce_triggerSz_sizeLim,

            # MIN stats
            'min_time_mean': np.mean(min_times) if min_times else None,
            'min_time_std': np.std(min_times) if min_times else None,
            'min_time_min': np.min(min_times) if min_times else None,
            'min_time_max': np.max(min_times) if min_times else None,
            
            # MAX stats
            'max_time_mean': np.mean(max_times) if max_times else None,
            'max_time_std': np.std(max_times) if max_times else None,
            'max_time_min': np.min(max_times) if max_times else None,
            'max_time_max': np.max(max_times) if max_times else None,
            
            # SUM stats 
            'sum_time_mean': np.mean(sum_times) if sum_times else None,
            'sum_time_std': np.std(sum_times) if sum_times else None,
            'sum_time_min': np.min(sum_times) if sum_times else None,
            'sum_time_max': np.max(sum_times) if sum_times else None,
        }
        
        return aggregated

    def connect_db(self):
        return psycopg2.connect(**self.db_config)

    def clean_tables(self, find_trigger="t_%"):
        '''drop all tables with wildcard match: find_trigger'''
        
        print(f"\nCleaning/ Dropping all Tables starting with '{find_trigger}'")
        with self.connect_db() as conn:
            with conn.cursor() as cur:
                try:
                    cur.execute(f"""SELECT tablename 
                                FROM pg_tables 
                                WHERE schemaname = 'public' AND tablename LIKE '{find_trigger}';""")
                    tables = cur.fetchall()

                    if not tables:
                        print(f"  No tables found matching: {find_trigger}")
                        return
                
                    for table in tables:
                        cur.execute(f"DROP TABLE {table[0]};")
                        print(f"  Dropping Table {table[0]}")

                except Exception as e:
                    print(f"    Error cleaning tables: {e}")
                    conn.rollback()

    def set_file_path(self, group_name: str, experiment_name:str) -> None:
        '''creates experiments root folder'''

        timestamp = time.strftime("d%d_m%m_y%Y")
        out_file = f'{timestamp}_{experiment_name}_sd{self.master_seed}'
        
        # prepend group in output path
        if group_name:
            experiment_folder_path = f'data/results/{group_name}/{out_file}'
        else:
            experiment_folder_path = f'data/results/{out_file}'
        
        self.resultFilepath = experiment_folder_path

    def save_results(self, experiment: ExperimentSettings):
        '''handles all experiment file outputs. (DDL, CSV results, Plots)
        **Does not handle DDL, DDL is handeled in run_experiment after generating data'''

        outputs = {}

        if self.results and experiment.save_csv:
            csv_path = self.__generate_csv_results(experiment.name)
            outputs['csv'] = csv_path
            print(f"  CSV saved: {csv_path}")

            if csv_path and experiment.independent_variable:
                plot_path = self.__generate_stats_results(csv_path, experiment.name, experiment.independent_variable)
                outputs['plot'] = plot_path
                print(f"  Plot saved: {plot_path}")
        
        return outputs

    def __generate_csv_results(self, experiment_name: str) -> str:
        '''save experiment results to CSV'''
        if not self.results:
            return 

        experiment_folder_path = self.resultFilepath
        
        # longer, more descriptive name, or shorter, easier name
        timestamp = time.strftime("d%d_m%m_y%Y")
        # out_file = f'{timestamp}_{experiment_name}_sd{self.master_seed}'
        out_file = f'results_sd{self.master_seed}'

        csv_path = f'{experiment_folder_path}/{out_file}.csv'  

        os.makedirs(experiment_folder_path, exist_ok=True)           
        df = pd.DataFrame(self.results)
        df.to_csv(csv_path, index=True)
        
        return csv_path
    
    def __generate_stats_results(self, csv_path:str, experiment_name: str, indep_variable: str):
        '''save experiment plot results. Use CSV data created right before this call'''
        
        experiment_folder_path = self.resultFilepath
        agg_out_file = f'aggregate_results_sd{self.master_seed}'
        combined_results_file = f'combined_agg_results_sd{self.master_seed}'
        agg_results_path =  f'{experiment_folder_path}/{agg_out_file}.jpg'
        combined_results_file = f'{experiment_folder_path}/{combined_results_file}.jpg'

        df = pd.read_csv(csv_path)
        fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(14, 5))
        
        # get the variable of the experiment #FIXME add in multi variable version & diff plots perhaps
        try:
            n = df[indep_variable]
        except Exception as e:
            return f"error trying to get x-axis:  {e}"
            
        min_mean_time = df['min_time_mean']
        max_mean_time = df['max_time_mean']
        sum_mean_time = df['sum_time_mean']

        # MIN
        ax1.errorbar(n, min_mean_time, yerr=df['min_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='purple')
        ax1.set_title("Mean Time of MIN", fontsize=14, fontweight='bold')
        ax1.set_xlabel(f'iv: {indep_variable}', fontsize=12)
        ax1.set_ylabel('Time (ms)', fontsize=12)
        ax1.grid(True, alpha=0.3)

        # MAX
        ax2.errorbar(n, max_mean_time, yerr=df['max_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='orange')
        ax2.set_title("Mean Time of MAX", fontsize=14, fontweight='bold')
        ax2.set_xlabel(f'iv: {indep_variable}', fontsize=12)
        ax2.set_ylabel('Time (ms)', fontsize=12)
        ax2.grid(True, alpha=0.3)

        # SUM
        ax3.errorbar(n, sum_mean_time, yerr=df['sum_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='green')
        ax3.set_title("Mean Time of SUM", fontsize=14, fontweight='bold')
        ax3.set_xlabel(f'iv: {indep_variable}', fontsize=12)
        ax3.set_ylabel('Time (ms)', fontsize=12)
        ax3.grid(True, alpha=0.3)

        plt.tight_layout()
        plt.savefig(agg_results_path)

        fig, ax = plt.subplots(figsize=(12, 5))

        ax.errorbar(n, min_mean_time, yerr=df['min_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='purple', label='MIN')
        ax.errorbar(n, max_mean_time, yerr=df['max_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='orange', label='MAX')
        ax.errorbar(n, sum_mean_time, yerr=df['sum_time_std'], marker='o', capsize=5, capthick=1, linewidth=2, markersize=5, color='green', label='SUM')

        ax.set_title(f"Query Performance vs {indep_variable})", fontsize=14, fontweight='bold')
        ax.set_xlabel(f'iv: {indep_variable}', fontsize=12)
        ax.set_ylabel('Time (ms)', fontsize=12)
        ax.legend(fontsize=11)
        ax.grid(True, alpha=0.3)
        plt.tight_layout()

        plt.tight_layout()
        plt.savefig(combined_results_file)

        return combined_results_file

    def __save_ddl_file(self, experiment: ExperimentSettings, data):
        ''' write data to DDL file for later loading 
            #NOTE broken. Need way to store final group and apppend all DDL to proper directory
        '''
        raise DeprecationWarning("Broken. Will fix if ever actually used. NOTE- Need way to store final group and append all DDL to proper directory. Currently it stores in group dirctory, but not the specific Experiment within this group.")

        experiment_folder_path = f'data/results/{self.groupName}/ddl'
        timestamp = time.strftime("d%d_m%m_y%Y")
        out_file = f'{timestamp}_{experiment.name}_sd{self.master_seed}'
        ddl_path = f'{experiment_folder_path}/{out_file}.sql'    

        os.makedirs(experiment_folder_path, exist_ok=True)           

        table_name = experiment.experiment_id
            
        with open(ddl_path, 'w') as file:
            if experiment.data_type == DataType.RANGE:
                file.write(f"CREATE TABLE {table_name} (id INT GENERATED ALWAYS AS IDENTITY, val int4range, mult int4range);\n\n")
            elif experiment.data_type == DataType.SET:
                file.write(f"CREATE TABLE {table_name} (id INT GENERATED ALWAYS AS IDENTITY, val int4range[], mult int4range);\n\n")
            
            batch_size = 250
            for i in range(0, len(data), batch_size):
                batch = data[i: i + batch_size]
                file.write(f"INSERT INTO {table_name} (val, mult) VALUES \n")

                values = []
                for val, mult in batch:                    
                    values.append(f"    ({val}, {mult})")

                file.write(',\n'.join(values))
                file.write(';\n\n')

        print(f"  DDL saved: {ddl_path}")

    def __generate_name(self, experiment: ExperimentSettings, generalName: bool = False) -> str:
        '''
            name format: t_experiment.name_{r | s}_{tX}_{master.seed}_{trial.seed}
            if generalName param is set, then trial number will be emit from result
        '''
        dtype = 'r' if experiment.data_type == DataType.RANGE else 's'

        if generalName:
            return f"t_{experiment.name}_s{self.master_seed}"

        seed_uid = str(self.trial_seed)[-5:]

        return f"t_{experiment.name}_t{experiment.curr_trial}_ms{self.master_seed}_ts{seed_uid}"
    
def run_all():
    ### Parse args and config
    args = parse_args()

    if args.seed:
        master_seed = generate_seed(args.seed)
    else:
        master_seed = generate_seed()
    print("The unique seed is ", master_seed)

    try:
        db_config = load_config(args.dbconfig)    
    except Exception as e:
        print(f"Error loading config: {e}")
        exit(1)

    ### Start engine
    runner = ExperimentRunner(db_config, master_seed)

    ### Clean before
    if args.clean_before:
        runner.clean_tables(args.clean_before)

    ### Load data: 3 branches of execution. Quick run, YAML file, python script.
    if args.quick:
        experiments = create_quick_experiment(args)
    elif args.yaml_experiments_file is not None:
        experiments = load_experiments_from_file(args.yaml_experiments_file)
    elif args.code:
        namespace = {'runner': runner, 'db_config': db_config,}
        exec(open(args.code).read(), namespace)
        experiments = namespace.get('experiments', {})

    ### Run every experiment and save results
    for group_name, ExpGroup in experiments.items(): 
        print(f"running experiment group:  {group_name}")

        # clear results buffer for each indep test group
        runner.results = []
        runner.name = ExpGroup.name
        runner.groupName = group_name

        for exp_name, experiment in ExpGroup.experiments.items():
            runner.set_file_path(group_name, exp_name)
            runner.run_experiment(experiment)
        
        results_paths = runner.save_results(experiment)

    ### Clean after
    if args.clean_after:
        runner.clean_tables(args.clean_after)

def generate_seed(in_seed=None):
    '''
        genrate the master seed of this programs run. (can be included in runner or settings class)
    '''
    if in_seed is not None:
        seed = in_seed
    else:
        seed = int(time.time() * 1000) % (2**32)
        
    random.seed(seed)
    np.random.seed(seed)
    
    return seed

def format_datasize(size):
        if size >= 1_000_000: 
            return str.replace(numerize.numerize(size, 2), '.', '_')
        return numerize.numerize(size, 0)

def format_name(experiment: ExperimentSettings):
    dtype = 's' if experiment.data_type == DataType.SET else 'r'
    sz = f"_n{format_datasize(experiment.dataset_size)}"
    unc = f"_unc{str.replace(str(experiment.uncertain_ratio), '.', '')}"
    # msr = f"{experiment.mult_size_range[0]}_{experiment.mult_size_range[1]}"
    iv = f"__iv_{experiment.iv_map[experiment.independent_variable]}"
    red_sz = f"_redSz{experiment.reduce_triggerSz_sizeLim[0]}_{experiment.reduce_triggerSz_sizeLim[1]}"
    ni_nir = ""
    gs_gsr = ""

    if experiment.num_intervals:
        ni_nir = f'_ni{experiment.num_intervals}'
    elif experiment.num_intervals_range:
        ni_nir = f'_nir{experiment.num_intervals_range[0]}_{experiment.num_intervals_range[1]}'

    if experiment.gap_size:
        gs_gsr = f'_gs{experiment.gap_size}'
    elif experiment.gap_size_range:
        gs_gsr = f'_gsr{experiment.gap_size_range[0]}_{experiment.gap_size_range[1]}'

    rv =  f"{dtype}{sz}{unc}{ni_nir}{gs_gsr}{red_sz}{iv}"
    return rv

if __name__ == '__main__':
    start = time.perf_counter()
    run_all()
    end = time.perf_counter()

    print(f"Tests took {end-start:.3f} s")

# python3 main.py --quick -dt r -nt 5 -sz 2 -ur .0 -ca        
# python3 main.py -xf tests_config.yaml -cb   