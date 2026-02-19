from cliUtility import *
from DataTypes import *
from numerize import numerize
from dataclasses import replace
from main import format_datasize, format_name, ExperimentGroup


'''
experiments is a dict of {str: ExperimentGroup}. ALlows for many unrelated experiments to run from 1 file
Naming convention is "GroupName/ID": {ExperimentGroup of related experiments}
persists in namespace of caller program
'''
experiments = dict()

# dummy used to access members
template = ExperimentSettings(
    data_type=DataType.SET, 
    dataset_size=10_000, 
    uncertain_ratio=0.0, 
    mult_size_range=(1,5),
    interval_size_range=(1, 1000), 
    num_intervals=4, 
    mode="all",
    num_trials=3, 
    gap_size_range=(0,100), 
    name= "temp",
    reduce_triggerSz_sizeLim=(10, 5),
)


def static_n_sweep(max_n: int = 11000, trigger_size: int = 10, reduce_to_size: int = 5):
    n_sweep = ExperimentGroup('n_sweep', 'dataset_size', None)
    for n in range(10_000, max_n+1, 50_000):
        experiment = replace(
            template,
            dataset_size             = n,
            num_trials               = 1,
            uncertain_ratio          = 0.0,
            independent_variable     = 'dataset_size',
            interval_size_range      = (1, 4_000),
            start_interval_range     = (1, 2),
            gap_size_range           = (250, 500),
            interval_width_range     = (2, 15),
            num_intervals            = 4,
            reduce_triggerSz_sizeLim = (trigger_size, reduce_to_size),
        )
        # experiment.name = f"n_{n}_{trigger_size}_{reduce_to_size}"
        experiment.name = f"n{n}_redSz{trigger_size}_{reduce_to_size}"
        n_sweep.experiments[experiment.name] = experiment

    return n_sweep

# experiments['n1mil_scaling_7_2'] = static_n_sweep(1_000_000, 7, 2)

experiments['n_scaling_15_10'] = static_n_sweep(100_000, 15, 10)
experiments['n_scaling_10_5'] = static_n_sweep(100_000, 10, 5)
experiments['n_scaling_4_2'] = static_n_sweep(100_000, 4, 2)
experiments['n_scaling_9_3'] = static_n_sweep(100_000, 9, 3)
experiments['n_scaling_5_2'] = static_n_sweep(100_000, 5, 2)
experiments['n_scaling_3_1'] = static_n_sweep(100_000, 3, 1)
experiments['n_scaling_1_1'] = static_n_sweep(100_000, 1, 1)



# static_n_sweep(100_000, 10, 5)
# static_n_sweep(50000, 10, 5)
