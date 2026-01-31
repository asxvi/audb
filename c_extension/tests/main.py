# naming q_n1000_i2
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
    name: str                       # required 
    data_type: DataType             # always Set or Range
    num_trials: int = 1             # always fixed
    dataset_size: int = 1           # always fixed
    uncertain_ratio: float = 0.00   # uncertainty ratio is split 50% in data, 50% in multiplicity columns. Uncert in data == NULL, uncert in mult = [0,N]
    
    # use these value if not None, otherwise use tuple if not None, both none = error
    num_intervals: int = None       
    gap_size: int = None

    # use if scalars are None and tuples are not None, both none = error
    num_intervals_range: tuple = None
    gap_size_range: tuple = None    
    mult_size_range: tuple = None   # required 
    interval_size_range: tuple = None

    make_csv: None = str          # always T or F
    mode: None = str      # always T or F

    insert_to_db = False

class ExperimentRunner:
    '''
        ExperimentRunner runs entire or parts of a test (gen_data, insert_db) by taking an ExperimentSettings
    '''
    def __init__(self, db_config, args):
        self.db_config = db_config
        self.results = []

        self.ddl_files = False

    def generate_data(self, experiment :ExperimentSettings, trial: int):
        '''
            Generates pseudorandom data based on user specified experiment settings. 
            * NOTE Specfic data serialization for different formats (i.e file and db ddl differs)
        '''
        # add reprod hash
        db_formatted_rows = []
        file_formatted_rows = []
        
        for i in range(experiment.dataset_size):
            if experiment.data_type == DataType.RANGE:
                obj = self.generate_range(experiment)
                val = str(obj) if not obj.isNone else None
            elif experiment.data_type == DataType.SET:
                obj = self.generate_set(experiment)
                val = str(obj) if (obj.rset and not getattr(obj, 'isNone', False)) else None

            mult_obj = self.generate_mult(experiment)
            mult = str(mult_obj)
            db_formatted_rows.append((val, mult))
            
            # change when cli utility
            if not experiment.insert_to_db:
                val = obj.str_ddl()
                mult = mult_obj.str_ddl()
                file_formatted_rows.append((val, mult))
                
        return db_formatted_rows, file_formatted_rows

    def run_experiment(self, experiment :ExperimentSettings) -> list:
        '''an experiement has N trials. gen data for each trial, run queries//benchmark results, and append to results'''    
        print(f"\n{'='*40}")
        print(f"Experiment: {experiment.name}")
        print(f"{'='*40}")
        
        trial_results = []

        with self.connect_db() as conn:
            with conn.cursor() as cur:
                for trial in range(experiment.num_trials):    
                    # seed_str = f"{experiment.name}_{trial+1}"
                    db_data_format, file_data_format = self.generate_data(experiment, trial+1)

                    if self.ddl_files or not experiment.insert_to_db:
                        print("file")
                        self.insert_data_file(experiment, trial+1, file_data_format)
                    else:
                        print("db")
                        self.insert_data_db(experiment, trial+1, db_data_format)
    
    def insert_data_db(self, experiment: ExperimentSettings, trial, data):
        '''Insert data into database specified in config file'''
        with self.connect_db() as conn:
            with conn.cursor() as cur:
                table_name = f"t_{experiment.name}_trial_{trial}"
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
        """Write data to SQL file for later loading
        
        * FIX filename when make name funcion works
        """

        os.makedirs(f'data/{experiment.name}', exist_ok=True)        
        table_name = f"t_{experiment.name}_trial_{trial}"
        filename = f"data/{experiment.name}/{table_name}.sql"
            
        with open(filename, 'w') as file:
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
        
    # generate an i4r. can also use psycopg.extras range type: https://www.psycopg.org/docs/extras.html#range-data-types
    def generate_range(self, experiment:ExperimentSettings) -> RangeType:
        # uncertain ratio. maybe should account for half nulls, half mult 0
        if np.random.random() < experiment.uncertain_ratio * 0.5:  
            return RangeType(0, 0, True)
        
        lb = np.random.randint(*experiment.interval_size_range)
        ub = np.random.randint(lb+1, experiment.interval_size_range[1]+1)
        return RangeType(lb, ub)
    
    def generate_set(self, experiment:ExperimentSettings) -> RangeSetType:
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

    def generate_mult(self, experiment:ExperimentSettings) -> RangeType:
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


def create_experiment_name(experiment: ExperimentSettings):
    dtype = 'r' if experiment.data_type == DataType.RANGE else 's'
    # seed = np.random.seed()
    return f"t_{experiment.name}_{dtype}_n{experiment.dataset_size}"

    # change up params, creating new experiment config...

def run_all():
    ### parse args and config
    args = parse_args()
    print(args)
    try:
        db_config = load_config(args.dbconfig)    
    except Exception as e:
        print(f"Error loading config: {e}")
        exit(1)

    ### start test engine with specific configuration
    runner = ExperimentRunner(db_config, args)

    # clean db before using
    if args.clean_before:
        runner.clean_tables(db_config, args.clean_before)


    # run experiments in experiment config.yaml, or based on flag input
    if args.quick:
        quick_experiment = create_quick_experiment(args)
    else:
        experiments = load_experiments_from_file(args)


    ### run experiments
    # create different trials objects we want to test with different parameters modifies
    # trial1 = ExperimentSettings(name="test1", data_type=DataType.RANGE, dataset_size=10, uncertain_ratio=0.1, mult_size_range=(1,5),
    #                             interval_size_range=(1, 100), num_intervals=2, num_intervals_range=(1,3), make_csv=False, insert_to_db=True,
    #                             num_trials=2, gap_size_range=(0,5), gap_size=None)

    # trial2 = ExperimentSettings(name="test2", data_type=DataType.SET, dataset_size=10, uncertain_ratio=0.3, mult_size_range=(1,5),
    #                             interval_size_range=(1, 100), num_intervals=2, num_intervals_range=(1,3), make_csv=False, insert_to_db=True,
    #                             num_trials=2, gap_size_range=(0,5), gap_size=None)
    
    # # create different trials objects we want to test with different parameters modifies
    # trial3 = ExperimentSettings(name="test3", data_type=DataType.RANGE, dataset_size=10, uncertain_ratio=0.1, mult_size_range=(1,5),
    #                             interval_size_range=(1, 100), num_intervals=2, num_intervals_range=(1,3), make_csv=False, insert_to_db=False,
    #                             num_trials=2, gap_size_range=(0,5), gap_size=None)

    # trial4 = ExperimentSettings(name="test4", data_type=DataType.SET, dataset_size=10, uncertain_ratio=0.3, mult_size_range=(1,5),
    #                             interval_size_range=(1, 100), num_intervals=2, num_intervals_range=(1,3), make_csv=False, insert_to_db=False,
    #                             num_trials=2, gap_size_range=(0,5), gap_size=None)
    
    # runner.run_experiment(trial1)
    # runner.run_experiment(trial2)
    # runner.run_experiment(trial3)
    # runner.run_experiment(trial4)

    # runner.clean_tables(db_config)


if __name__ == '__main__':    
    run_all()


# python3 main.py --quick -dt r -nt 5 -sz 200 -ur .40 -nir (1,5) -gsr (0, 10) -msr (0, 5) -isr (1, 200) -csv -ddl -cb -ca 