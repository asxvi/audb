from collections import defaultdict
from cliUtility import *
from DataTypes import *
from dataclasses import replace
from main import ExperimentGroup, format_name, ExperimentSuite

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

def static_n_sweep(max_n: int = 100_000, step: int = 10_000, trigger_size: int = 10, reduce_to_size: int = 5):
    group = ExperimentGroup(f'n{max_n}_red{trigger_size}_{reduce_to_size}_sweep', 'dataset_size', None)

    for n in range(step, max_n+step, step):
        experiment = replace(
            template,
            dataset_size             = n,
            num_trials               = 5,
            uncertain_ratio          = 0.0,
            independent_variable     = 'dataset_size',
            interval_size_range      = (1, 4_000),
            start_interval_range     = (1, 2),
            gap_size_range           = (250, 500),
            interval_width_range     = (2, 15),
            num_intervals            = 4,
            reduce_triggerSz_sizeLim = (trigger_size, reduce_to_size),
        )
        experiment.name = format_name(experiment)
        group.experiments[experiment.name] = experiment
    
    return group

def plot_all_n_scale(n:int, step:int, suite_name:str = None):
    suite_name = suite_name if suite_name is not None else 'n_sweeping'
    if suite_name not in experiments:
        experiments[suite_name] = ExperimentSuite(suite_name)
    
    suite = experiments[suite_name]
    
    suite.add(static_n_sweep(n, step, 15, 10))
    suite.add(static_n_sweep(n, step, 10, 5))
    suite.add(static_n_sweep(n, step, 4, 2))
    suite.add(static_n_sweep(n, step, 9, 3))
    suite.add(static_n_sweep(n, step, 5, 2))
    suite.add(static_n_sweep(n, step, 3, 1))
    suite.add(static_n_sweep(n, step, 1, 1))

# plot_all_n_scale(10_000, 1000, 'n_sweeping100k')
plot_all_n_scale(400, 40, 'n_sweeping400k')

def num_intervals_sweep(max_ni: int = 4, n: int = 10_000):
    group = ExperimentGroup(f'ni{max_ni}_n{n}_sweep', 'num_intervals', None)
    
    for ni in range(1, max_ni+1, 1):
        experiment = replace(
            template,
            dataset_size             = n,
            num_trials               = 3,
            uncertain_ratio          = 0.0,
            independent_variable     = 'num_intervals',
            interval_size_range      = (1, 50_000),
            start_interval_range     = (1, 2),
            gap_size_range           = (1000, 5000),
            interval_width_range     = (2, 15),
            num_intervals            = ni,
            reduce_triggerSz_sizeLim = (5, 2),
        )

        experiment.name = f"ni{ni}_n{n}"
        group.experiments[experiment.name] = experiment

    return group

def plot_all_ni_sweep(max_ni, n):
    suite = ExperimentSuite('ni_sweeping')
    # experiments['n_scaling_15_10'] = static_n_sweep(n, step, 15, 10)
    # experiments['n_scaling_10_5'] = static_n_sweep(n, step, 10, 5)
    # experiments['n_scaling_4_2'] = static_n_sweep(n, step, 4, 2)
    # experiments['n_scaling_9_3'] = static_n_sweep(n, step, 9, 3)
    # experiments['n_scaling_5_2'] = static_n_sweep(n, step, 5, 2)
    
    suite.add(num_intervals_sweep(max_ni, n))
    

    experiments[suite.name] = suite

# plot_all_ni_sweep(1, 100)
# plot_all_n_scale(40_000, 4_000)
# plot_all_n_scale(100_000, 10_000)


# for fn, args in [
#     (plot_all_n_scale, (100, 10)),
#     (plot_all_n_scale, (400, 40)),
# ]:
#     fn(*args)