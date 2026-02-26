from collections import defaultdict
from cliUtility import *
from DataTypes import *
from dataclasses import replace
from main import ExperimentGroup, format_datasize, format_name, ExperimentSuite

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

def ni_gap_sweep(gap_sizes, max_ni: int = 10, n: int = 10_000, trigger_size: int = 10, reduce_to_size: int = 5):
    group = ExperimentGroup(f'ni_gap_red{trigger_size}_{reduce_to_size}_sweep', 'num_intervals', None)
    
    for g in gap_sizes:
        for ni in range(1, max_ni+1):
            experiment = replace(
                template,
                dataset_size             = n,
                num_trials               = 5,
                uncertain_ratio          = 0.0,
                independent_variable     = 'num_intervals',
                interval_size_range      = (1, 100_000),
                start_interval_range     = (1, 2),
                # gap_size_range           = (g, g+1),  # fix the gap size
                gap_size                 = g,
                interval_width_range     = (2, 15),
                num_intervals            = ni,
                reduce_triggerSz_sizeLim = (trigger_size, reduce_to_size),
            )
            experiment.name = f"{format_name(experiment)}_g{g}"  # make sure gap_size is in name to avoid overwrite
            group.experiments[experiment.name] = experiment
    
    return group

# ================ #


def plot_ni_gap_sweep(max_ni: int, n: int, suite_name: str = None):
    ''' '''
    suite_name = suite_name if suite_name is not None else f'ni_gap_sweeping{format_datasize(n)}'
    if suite_name not in experiments:
        experiments[suite_name] = ExperimentSuite(suite_name)
    
    gap_sizes = [10, 50, 100, 500, 1000, 10000, 20000, 40000, 80000]
    # gap_sizes = [10, 50, 100, 250, 500, 10000, 20000]
    # experiments[suite_name].add(ni_gap_sweep(gap_sizes, max_ni, n, 15, 10))
    # experiments[suite_name].add(ni_gap_sweep(gap_sizes, max_ni, n, 10, 5))
    # experiments[suite_name].add(ni_gap_sweep(gap_sizes, max_ni, n, 4, 2))
    # experiments[suite_name].add(ni_gap_sweep(gap_sizes, max_ni, n, 9, 3))
    # experiments[suite_name].add(ni_gap_sweep(gap_sizes, max_ni, n, 5, 2))
    # experiments[suite_name].add(ni_gap_sweep(gap_sizes, max_ni, n, 1, 1))
    experiments[suite_name].add(ni_gap_sweep(gap_sizes, max_ni, n, 3, 1))


## ============================== ##
plot_ni_gap_sweep(10, 10000, 'ni_gap_sweep_n10k')