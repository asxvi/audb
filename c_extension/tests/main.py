# naming q_n1000_i2
import random
import os
import psycopg2
import psycopg2.extras
import numpy as np
from dataclasses import dataclass

from cliUtility import *
from DataTypes import *

@dataclass
class ExperimentSettings:
    '''
        Class contains the modifiable settings of a test
        if num_intervals is used, num_intervals_range shouldn't be used
        if gap_size is used, gap_size_range shouldn't be used
    '''
    name: str                           # required 
    data_type: DataType                 # always Set or Range
    experiment_id: str = None           # unique string name that identifies specific experiment
    num_trials: int = 1                 # always fixed
    dataset_size: int = 100             # always fixed
    uncertain_ratio: float = 0.00       # uncertainty ratio is split 50% in data, 50% in multiplicity columns. Uncert in data == NULL, uncert in mult = [0,N]
    interval_size_range: tuple = (1, 100)
    mult_size_range: tuple = (1,5)      # required 
    
    # use these value if not None, otherwise use tuple if not None, both none = error
    num_intervals: int = None       
    gap_size: int = None
    # use if scalars are None and tuples are not None, both none = error
    num_intervals_range: tuple = None
    gap_size_range: tuple = None    

    mode: str = None                # what modes of test suite to execute
    save_ddl:bool = False           # store ddl code to make tables 
    save_csv: bool = False          # store csv with statistics and results of test
    # insert_to_db: bool = False      # this is actually stupid, but currently code relies on this


class ExperimentRunner:
    '''
        ExperimentRunner runs entire or parts of a test (gen_data, insert_db) by taking an ExperimentSettings
    '''
    def __init__(self, db_config, seed):
        self.db_config = db_config
        self.results = []
        self.master_seed = seed
        self.trial_seed = None

    def generate_data(self, experiment :ExperimentSettings, trial: int):
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

    def run_experiment(self, experiment :ExperimentSettings) -> list:
        '''an experiement has N trials. gen data for each trial, run queries//benchmark results, and append to results'''    
        trial_results = []

        # generate data for each trial. Insert ddl to file optinally. After inserting to DB, run tests
        with self.connect_db() as conn:
            with conn.cursor() as cur:
                for trial in range(experiment.num_trials):    
                    
                    # create a trial seed dependent on the master seed, and specific trial number
                    self.trial_seed = (self.master_seed + trial + 1) % (2**32)
                    np.random.seed(self.trial_seed)
                    experiment.experiment_id = self.__generate_name(experiment, trial+1)

                    db_data_format, file_data_format = self.generate_data(experiment, trial+1)

                    # save in ddl compatible format. Insert into DB regardless... need to run tests
                    if experiment.save_ddl:
                        print("ddl")
                        self.insert_data_file(experiment, trial+1, file_data_format)
                    
                    self.insert_data_db(experiment, trial+1, db_data_format)
    
    def insert_data_db(self, experiment: ExperimentSettings, trial, data):
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

    def insert_data_file(self, experiment: ExperimentSettings, trial, data):
        """
            Write data to SQL file for later loading
        """

        experiment_folder_path = f'data/{experiment.name + "_s" + str(self.master_seed)}'
        table_name = experiment.experiment_id
        os.makedirs(experiment_folder_path, exist_ok=True)        
        filepath = f"{experiment_folder_path}/{table_name}.sql"
            
        with open(filepath, 'w') as file:
            if experiment.data_type == DataType.RANGE:
                file.write(f"CREATE TABLE {table_name} (id INT GENERATED ALWAYS AS IDENTITY, val int4range, mult int4range);\n\n")
            elif experiment.data_type == DataType.SET:
                file.write(f"CREATE TABLE {table_name} (id INT GENERATED ALWAYS AS IDENTITY, val int4range[], mult int4range);\n\n")
            
            batch_size = 100
            for i in range(0, len(data), batch_size):
                batch = data[i: i + batch_size]
                file.write(f"INSERT INTO {table_name} (val, mult) VALUES \n")

                values = []
                for val, mult in batch:                    
                    values.append(f"    ({val}, {mult})")

                file.write(',\n'.join(values))
                file.write(';\n\n')
        
    def __generate_range(self, experiment:ExperimentSettings) -> RangeType:
        # uncertain ratio. maybe should account for half nulls, half mult 0
        if np.random.random() < experiment.uncertain_ratio * 0.5:  
            return RangeType(0, 0, True)

        lb = np.random.randint(*experiment.interval_size_range)
        ub = np.random.randint(lb+1, experiment.interval_size_range[1]+1)
        return RangeType(lb, ub)
    
    def __generate_set(self, experiment:ExperimentSettings) -> RangeSetType:
        # if experiment.num_intervals is not None then use, otherwise if experiment.num_intervals_range then use. otherwise raise error
        if experiment.num_intervals is not None:
            num_intervals = experiment.num_intervals
        elif experiment.num_intervals_range is not None:
            num_intervals = np.random.randint(*experiment.num_intervals_range)
        else:
            raise ValueError()
        
        rset = []
        for i in range(num_intervals):    
            # uncertain ratio. maybe should account for half nulls, half mult 0
            if np.random.random() < experiment.uncertain_ratio * 0.5:  
                rset.append(RangeType(0,0,True))
                continue
            
            lb = np.random.randint(*experiment.interval_size_range)
            ub = np.random.randint(lb+1, experiment.interval_size_range[1]+1)
            
            rset.append(RangeType(lb,ub,False))
        
        return RangeSetType(rset, cu=False)

    def __generate_mult(self, experiment:ExperimentSettings) -> RangeType:
        # uncertain ratio. maybe should account for half nulls, half mult 0
        if np.random.random() < experiment.uncertain_ratio * 0.5:  
            return RangeType(0, 0, True)
        
        lb = np.random.randint(*experiment.mult_size_range)
        ub = np.random.randint(lb+1, experiment.mult_size_range[1]+1)
        return RangeType(lb, ub)
    
    def connect_db(self):
        return psycopg2.connect(**self.db_config)

    def clean_tables(self, config, find_trigger="t_%"):
        print(f"Cleaning/ Dropping all Tables starting with '{find_trigger}'")
        dbname = config['database']

        with self.connect_db() as conn:
            with conn.cursor() as cur:
                try:
                    cur.execute(f"""SELECT tablename 
                                FROM pg_tables 
                                WHERE schemaname = 'public' AND tablename LIKE '{find_trigger}';""")
                    tables = cur.fetchall()

                    for table in tables:
                        cur.execute(f"DROP TABLE {table[0]};")
                        print(f" Dropping Table {table[0]}")

                except Exception as e:
                    print(f"Error cleaning tables: {e}")
                    conn.rollback()

    def __generate_name(self, experiment: ExperimentSettings, trialNum: int) -> str:
        '''
            name format: t_experiment.name_{r | s}_{tX}_{master.seed}_{trial.seed}
        '''
        dtype = 'r' if experiment.data_type == DataType.RANGE else 's'

        return f"t_{experiment.name}_{dtype}_t{trialNum}_s{self.master_seed}_ts{self.trial_seed}"

def run_all():
    ### parse args and config
    args = parse_args()

    if args.seed:
        master_seed = generate_seed(args.seed)
    else:
        master_seed = generate_seed()
    
    try:
        db_config = load_config(args.dbconfig)    
    except Exception as e:
        print(f"Error loading config: {e}")
        exit(1)

    ### start test engine with specific configuration
    runner = ExperimentRunner(db_config, master_seed)

    # clean db before using
    if args.clean_before:
        runner.clean_tables(db_config, args.clean_before)

    # run experiments in experiment config.yaml, or based on flag input
    if args.quick:
        experiments = create_quick_experiment(args)
    else:
        experiments = load_experiments_from_file(args.experiments_file)

    # Only run specific mode of ExperimentRunner
    for name, experiment in experiments.items():
        # experiment.name = runner.__generate_name(experiment)
        runner.run_experiment(experiment)


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


if __name__ == '__main__':    
    run_all()

# main.py --quick -dt r -nt 5 -sz 2 -ur .0 -ca        