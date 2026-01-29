# naming q_n1000_i2

import itertools
import pandas as pd
import psycopg2
import psycopg2.extras
from configparser import ConfigParser
from dataclasses import dataclass
import numpy as np
from enum import Enum

class DataType(Enum):
    RANGE = "range"
    SET = "rset"

'''
    local represention of postres RangeType. helper methods include arithmetic,
    logical operators, and convenience methods
'''
class RangeType:
    def __init__(self, lb=0, ub=0, isNone=False):
        assert(lb<=ub)
        self.lb = lb
        self.ub = ub
        self.isNone = isNone
        
    def __add__(self, o):
        if isinstance(o, self.__class__):
            return RangeType(self.lb+o.lb, self.ub+o.ub)
        elif isinstance(o, int):
            return RangeType(self.lb+o, self.ub+o)
        else:
            raise TypeError("unsupported operand type(s) for +: '{}' and '{}'").format(self.__class__, type(other))
        
    
    def __and__(self, o):
        if not (isinstance(self.lb, bool) and isinstance(self.ub, bool) and
                isinstance(o.lb, bool) and isinstance(o.ub, bool)):
            raise ValueError("Both operands must be RangeType objects with boolean bounds.")
        
        # Calculate the new bounds
        new_lb = self.lb and o.lb
        new_ub = self.ub and o.ub
        
        return RangeType(new_lb, new_ub)
    
    def __or__(self, o):
        if not (isinstance(self.lb, bool) and isinstance(self.ub, bool) and
                isinstance(o.lb, bool) and isinstance(o.ub, bool)):
            raise ValueError("Both operands must be RangeType objects with boolean bounds.")
        
        # Calculate the new bounds
        new_lb = self.lb or o.lb
        new_ub = self.ub or o.ub
        
        return RangeType(new_lb, new_ub)
    
    def __mul__(self, o):
        if isinstance(o, self.__class__):
            lb = min(self.lb*o.lb, self.lb*o.ub, self.ub*o.lb, self.ub*o.ub)
            ub = max(self.lb*o.lb, self.lb*o.ub, self.ub*o.lb, self.ub*o.ub)
            return RangeType(lb, ub)
        elif isinstance(o, int):
            lb = min(self.lb*o, self.ub*o)
            ub = max(self.lb*o, self.ub*o)
            return RangeType(lb, ub)
        else:
            raise TypeError("unsupported operand type(s) for *: '{}' and '{}'").format(self.__class__, type(other))
    
    def __hash__(self):
        # Combine the lower and upper bounds into a hashable representation
        return hash(self.lb, self.ub)
    
    def __eq__(self, o):
        return RangeType(self.lb==self.ub and self.lb==o.lb and o.lb==o.ub, self.i(o) is not None)

    def __gt__(self, o):
        return RangeType(self.lb > o.ub, self.ub > o.lb)

    def __ge__(self, o):
        return RangeType(self.lb >= o.ub, self.ub >= o.lb)
    
    def __lt__(self, o):
        return RangeType(self.lb < o.ub, self.ub < o.lb)

    def __le__(self, o):
        return RangeType(self.lb <= o.ub, self.ub <= o.lb)

    def u(self, o):
        return RangeType(min(self.lb,o.lb), max(self.ub,o.ub))
    
    def i(self, o):
        lb = max(self.lb,o.lb)
        ub = min(self.ub,o.ub)
        if lb <= ub:
            return RangeType(max(self.lb,o.lb), min(self.ub,o.ub))
        return None
    
    def __eq__(self, other):
        if self.ub >= other.lb and other.ub >= self.lb:
            return True
        return False
    
    def __repr__(self):
        return f"[{self.lb}, {self.ub}]"
    
    def __str__(self):
        return f"[{self.lb}, {self.ub}]"

    # easier to work in postgres    
    def str_ddl(self):
        return f"int4range({self.lb}, {self.ub})"
    
    # to be used only for local development and testing. 
    def generate_range(self, experiment:ExperimentSettings) -> RangeType:
        # uncertain ratio. maybe should account for half nulls, half mult 0
        if np.random.random() < experiment.uncertain_ratio * 0.5:  
            return RangeType(0,0,True)
        
        lb = np.random.randint(*experiment.interval_size_range)
        ub = np.random.randint(lb+1, experiment.interval_size_range[1]+1)
        return RangeType(lb, ub)

'''
    local represention of postres ArrayType. helper methods include arithmetic,
    logical operators, and convenience methods
'''
class RangeSetType:
    def __init__(self, rset, vtype = RangeType, cu=True):
        assert(type(rset) is list)
        self.vtype = vtype
        self.rset = rset
        if cu:
            self.cleanup()
        
    def __repr__(self):
        return f"{self.rset}"
    
    def __str__(self):
        return f"{self.rset}"
        # items = [f'"{str(i)}"' for i in self.rset]
        # return "{" + ",".join(items) + "}"
    
    # easier to work in postgres    
    def str_ddl(self):
        items = [f"{i.str_ddl()}" for i in self.rset]
        return "array[" + ",".join(items) + "]"
    
    def __len__(self):
        return len(self.rset)
    
    def cleanup(self):
        if self.vtype == RangeType:
            if len(self)<=1:
                return
            l = sorted(self.rset, key=lambda r: r.lb)
            res = []
            curR = l[0]
            for rv in sorted(self.rset, key=lambda r: r.lb):
                if rv.lb <= curR.ub:
                    curR = curR.u(rv)
                else:
                    res.append(curR)
                    curR = rv
            res.append(curR)
            self.rset = res
        
    # +
    def __add__(self, o, cu=True):
        rst = []
        if isinstance(o, self.__class__):
            for p in itertools.product(self.rset,o.rset):
                rst.append(p[0]+p[1])
            rt = RangeSetType(rst)
            if cu:
                rt.cleanup()
            return rt
        elif isinstance(o, int):
            for p in self.rset:
                rst.append(p+o)
            rt = RangeSetType(rst)
            if cu:
                rt.cleanup()
            return rt
        else:
            raise TypeError("unsupported operand type(s) for +: '{}' and '{}'").format(self.__class__, type(other))
        
    # *
    def __mul__(self, o, cu=True):
        rst = []
        if isinstance(o, self.__class__):
            for p in itertools.product(self.rset,o.rset):
                rst.append(p[0]*p[1])
            rt = RangeSetType(rst)
            if cu:
                rt.cleanup()
            return rt
        elif isinstance(o, int):
            for p in self.rset:
                rst.append(p*o)
            rt = RangeSetType(rst)
            if cu:
                rt.cleanup()
            return rt
        else:
            raise TypeError("unsupported operand type(s) for *: '{}' and '{}'").format(self.__class__, type(other))

    def __eq__(self, o):
        lb = False
        if len(self)==1 and len(o)==1: 
            l = next(iter(self.rset))
            r = next(iter(o.rset))
            lb = l.lb==l.ub and l.lb==r.lb and r.lb==r.ub
        return RangeType(lb, bool(self.i(o)))

    def u(self, o, cu=True):
        rst = []
        for p in itertools.product(self.rset,o.rset):
            rst.append(p[0].u(p[1]))
        rt = RangeSetType(rst)
        if cu:
            rt.cleanup()
        return rt
    
    def i(self, o, cu=True):
        rst = []
        for p in itertools.product(self.rset,o.rset):
            ir = p[0].i(p[1])
            if ir is not None:
                rst.append(p[0].i(p[1]))
        rt = RangeSetType(rst)
        if cu:
            rt.cleanup()
        return rt
    
    def lb(self):
        return sorted(self.rset, key=lambda r: r.lb)[0].lb
    
    def ub(self):
        return sorted(self.rset, key=lambda r: r.ub, reverse=True)[0].ub
    
    def itv(self):
        return RangeType(self.lb(),self.ub())
    
    def __gt__(self, o):
        if isinstance(o, int):
            return self.itv() > RangeType(o, o)
        return self.itv() > o.itv()

    def __ge__(self, o):
        if isinstance(o, int):
            return self.itv() >= RangeType(o, o)
        return self.itv() >= o.itv()
    
    def __lt__(self, o):
        if isinstance(o, int):
            return self.itv() < RangeType(o, o)
        return self.itv() < o.itv()

    def __le__(self, o):
        if isinstance(o, int):
            return self.itv() <= RangeType(o, o)
        return self.itv() <= o.itv()
    
    def generate_set(self, experiment:ExperimentSettings) -> RangeSetType:
        num_ranges = np.random.randint(*experiment.num_intervals_range)
        
        rset = []
        for i in range(num_ranges):    
            # uncertain ratio. maybe should account for half nulls, half mult 0
            if np.random.random() < experiment.uncertain_ratio * 0.5:  
                rset.append(RangeType(0,0,True))
                continue
            
            lb = np.random.randint(*experiment.interval_size_range)
            ub = np.random.randint(lb+1, experiment.interval_size_range[1]+1)
            
            rset.append(RangeType(lb,ub,False))
            print(RangeType(lb,ub,False))
        
        return RangeSetType(rset, cu=False)

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
    make_csv:bool                   ####
    insert_to_db:bool               ####

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
    def generate_range(self, experiment:ExperimentSettings) -> RangeType:
        # uncertain ratio. maybe should account for half nulls, half mult 0
        if np.random.random() < experiment.uncertain_ratio * 0.5:  
            return None
        
        lb = np.random.randint(*experiment.interval_size_range)
        ub = np.random.randint(lb+1, experiment.interval_size_range[1]+1)
        return RangeType(lb, ub)
    
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
            return RangeType(0, upper)
        
        lb = np.random.randint(*experiment.mult_size_range)
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
                                interval_size_range=(1, 1000), num_intervals=2, num_intervals_range=(1,3), make_csv=False, insert_to_db=True,
                                num_trials=2, gap_size_range=(0,5))

    trial2 = ExperimentSettings(name="test2", data_type=DataType.RANGE, dataset_size=17, uncertain_ratio=0.3, mult_size_range=(1,5),
                                interval_size_range=(1, 100), num_intervals=2, num_intervals_range=(1,3), make_csv=False, insert_to_db=True,
                                num_trials=2, gap_size_range=(0,5))
    
    # run the experiment for each of the trial objects.
        # inside run_experiment: 
            # for every trial in Trial
            # generate data (with reproducibilty)
    # runner.run_experiment(trial1)
    # runner.run_experiment(trial2)   


def create_experiment_name(experiment: ExperimentSettings):
    dtype = 'r' if experiment.data_type == DataType.RANGE else 's'
    # seed = np.random.seed()
    return f"t_{experiment.name}_{dtype}_n{experiment.dataset_size}"

    # change up params, creating new experiment config...

if __name__ == '__main__':
    run_all()