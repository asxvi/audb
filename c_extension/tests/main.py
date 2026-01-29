# naming q_n1000_i2

import pandas as pd
import psycopg2
import psycopg2.extras
from configparser import ConfigParser
from dataclasses import dataclass
import numpy as np
from enum import Enum

class DataType(Enum):
    RANGE = "range"
    SET = "set"


@dataclass
class ExperimentSettings:
    name: str
    data_type: DataType
    num_trials: int
    dataset_size: int
    uncertain_ratio: float              #uncertainty can be thru mult 0, or missing vals in col.. (more?). we will generate both cases
    mult_size_range: tuple
    interval_size_range: tuple
    num_intervals_range: tuple
    num_intervals: int
    gap_size_range: tuple

class ExperimentRunner:
    def __init__(self, db_config):
        self.db_config = db_config
        self.results = []

    '''
        an experiement has N trials. gen data for each trial, run queries//benchmark results, and append to results
    '''
    def run_experiment(self, experiment :ExperimentSettings) -> list:
        print(f"\n{'='*40}")
        print(f"Experiment: {experiment.name}")
        print(f"{'='*40}")
        
        trial_results = []
        
        with self.connect_db() as conn:
            with conn.cursor() as cur:
                for trial in range(experiment.num_trials):    
                    # seed_str = f"{experiment.name}_{trial+1}"

                    rows = self.generate_data(experiment, trial+1)
                    print(rows)
            
        # np.random.seed()


    # generates pseudorandom 'fake' data based on user specified values.
    def generate_data(self, experiment :ExperimentSettings, trial: int):
        with self.connect_db() as conn:
            with conn.cursor() as cur:
                table_name = f"t_{experiment.name}_trial_{trial}"
                
                cur.execute(f"DROP TABLE IF EXISTS {table_name};")
                
                if experiment.data_type == DataType.RANGE:
                    cur.execute(f"CREATE TABLE {table_name} (id INT GENERATED ALWAYS AS IDENTITY, val int4range, mult int4range);")                
                elif experiment.data_type == DataType.SET:
                    cur.execute(f"CREATE TABLE {table_name} (id INT GENERATED ALWAYS AS IDENTITY, val int4range[], mult int4range);")
                
                # add reprod hash

                rows = []
                for i in range(experiment.dataset_size):
                    if experiment.data_type == DataType.RANGE:
                        val = self.generate_range(experiment)
                        val = '[0,0)' if val is None else val
                    elif experiment.data_type == DataType.RANGE:
                        # val = self.generate_set(experiment)
                        # val = 'NULL' if val is None else val
                        val = '[0,0)' if val is None else val
                    
                    mult = self.generate_mult(experiment)
                    rows.append((val, mult))    

                if experiment.data_type == DataType.RANGE:
                    template = '(%s::int4range, %s::int4range)'
                else:
                    template = '(%s::int4range[], %s::int4range)'

                sql = f"INSERT INTO {table_name} (val, mult) VALUES %s"
                psycopg2.extras.execute_values(cur, sql, rows, template)

                conn.commit()
        return table_name

    # generate an i4r. can also use psycopg.extras range type: https://www.psycopg.org/docs/extras.html#range-data-types
    def generate_range(self, experiment:ExperimentSettings) -> str:
        # uncertain ratio. maybe should account for half nulls, half mult 0
        if np.random.random() < experiment.uncertain_ratio * 0.5:  
            return None
        
        lb = np.random.randint(experiment.interval_size_range[0], experiment.interval_size_range[1])
        ub = np.random.randint(lb+1, experiment.interval_size_range[1]+1)
        return f'[{lb}, {ub})'
    
    # def generate_set(self, experiment:ExperimentSettings) -> str:
        num_ranges = np.random.randint(*experiment.num_intervals_range)
        
        # for i in range(experiment)
        
        # uncertain ratio. maybe should account for
        is_uncertain = np.random.random() < experiment.uncertain_ratio
        if (is_uncertain):
            return None
        
        lb = np.random.randint(experiment.interval_size_range[0], experiment.interval_size_range[1])
        ub = np.random.randint(lb+1, experiment.interval_size_range[1]+1)
        return f'array[{lb}, {ub})'

    def generate_mult(self, experiment:ExperimentSettings) -> str:
        # uncertain ratio. maybe should account for half nulls, half mult 0
        if np.random.random() < experiment.uncertain_ratio * 0.5:  
            upper = np.random.randint(1, experiment.mult_size_range[1]+1)
            return f'[{0}, {upper})'
        
        lb = np.random.randint(experiment.mult_size_range[0], experiment.mult_size_range[1])
        ub = np.random.randint(lb+1, experiment.mult_size_range[1]+1)
        return f'[{lb}, {ub})'

    def connect_db(self):
        return psycopg2.connect(**self.db_config)

    
def load_config(filename='database.ini', section='postgresql'):
    parser = ConfigParser()
    parser.read(filename)

    config = {} 
    if parser.has_section(section):
        params = parser.items(section)
        for param in params:
            config[param[0]] = param[1]
    else:
        raise Exception('Section {0} not found in the {1} file'.format(section, filename))
    return config


def run_all():
    try:
        db_config = load_config()    
    except Exception as e:
        print(f"Error loading config: {e}")
        exit(1)


    runner = ExperimentRunner(db_config)

    # create different trials objects we want to test with different parameters modifies
    trial1 = ExperimentSettings(name="test1", data_type=DataType.RANGE, dataset_size=10, uncertain_ratio=0.1, mult_size_range=(1,5),
                                interval_size_range=(1, 30), num_intervals=2, num_intervals_range=(1,3), 
                                num_trials=2, gap_size_range=(0,5))

    # run the experiment for each of the trial objects.
        # inside run_experiment: 
            # for every trial in Trial
            # generate data (with reproducibilty)
    runner.run_experiment(trial1)





    # change up params, creating new experiment config

# name: str
# dataset_size: int
# uncertain_ratio: float
# interval_size_range: tuple
# num_intervals: int
# num_intervals_range: tuple
# num_trials: int
# gap_size_range: tuple

if __name__ == '__main__':
    run_all()