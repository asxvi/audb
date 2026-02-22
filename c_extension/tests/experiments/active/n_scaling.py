from cliUtility import *
from DataTypes import *
from dataclasses import replace
from main import ExperimentGroup, format_name


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

def make_wide_interval(n:int, k:int = 4):
    width_max = 15
    gap_min = int(n * k * width_max * 2)
    gap_max = gap_min * 4
    
    return {
        'interval_size_range':  (1, 1_000_000_000),
        'start_interval_range': (1, 1_000),
        'gap_size_range':       (gap_min, gap_max),
        'interval_width_range': (2, width_max),
    }

def make_small_interval(n:int, k:int = 4):
    width_max = 15
    gap_min = int(n * k * width_max * 2)
    gap_max = gap_min * 4
    
    return {
        'interval_size_range':  (1, 4_000),
        'start_interval_range': (1, 40),
        'gap_size_range':       (gap_min, gap_max),
        'interval_width_range': (2, width_max),
    }

def static_n_sweep(max_n: int = 100_000, step: int = 10_000, trigger_size: int = 10, reduce_to_size: int = 5):
    n_sweep = ExperimentGroup('n_sweep', 'dataset_size', None)
    for n in range(1, max_n+1, step):
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
        n_sweep.experiments[experiment.name] = experiment

    return n_sweep

def num_intervals_sweep(max_ni: int = 4, n: int = 10_000):
    group = ExperimentGroup('ni_sweep', 'num_intervals', None)
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

def reduction_sweep(n: int = 100_000):
    red_sweep = ExperimentGroup('reduction_sweep50k', 'reduce_triggerSz_sizeLim', None)
    configs = [
        (15,10),
        (10,5),
        (9,3),
        (7,2),
        (5,2),
        (3,1),
        (1,1),
    ]

    for trigger_size, reduce_to_size in configs:
        experiment = replace(
            template,
            dataset_size             = n,
            num_trials               = 5,
            uncertain_ratio          = 0.0,
            independent_variable     = 'reduce_triggerSz_sizeLim',
            interval_size_range      = (1, 50_000),
            start_interval_range     = (1, 2),
            gap_size_range           = (1000, 5000),
            interval_width_range     = (2, 15),
            num_intervals            = 4,
            reduce_triggerSz_sizeLim = (trigger_size, reduce_to_size),
        )

        experiment.name = f"red_{trigger_size}_{reduce_to_size}"
        red_sweep.experiments[experiment.name] = experiment

    return red_sweep

def gap_vs_interval_size(max_gap: int = 40, interval_size=4000, num_points: int = 10):
    """ sweep different gap sizes for a fixed interval size """

    group = ExperimentGroup('gap_sweep', 'gap_size', None)

    gaps = list(range(1, max_gap + 1, max(1, max_gap // num_points)))
    for gs in gaps:
        experiment = replace(
            template,
            dataset_size             = 10_000,
            num_trials               = 3,
            uncertain_ratio          = 0.0,
            independent_variable     = 'num_intervals',
            interval_size_range      = (1, interval_size),
            start_interval_range     = (1, 2),
            gap_size                 = gs,
            # gap_size_range           = (1000, 5000),
            interval_width_range     = (2, 15),
            num_intervals            = 4,
            reduce_triggerSz_sizeLim = (5, 2),
        )

        experiment.name = f"gs{gs}_isr{interval_size}"
        group.experiments[experiment.name] = experiment

    return group


# gap_vs_interval_size(40, 4000, 10)

# gap_vs_interval_size(10_0000, 1_000_000_000)



    
def plot_all_n_scale(n, step):
    experiments['n_scaling_15_10'] = static_n_sweep(n, step, 15, 10)
    experiments['n_scaling_10_5'] = static_n_sweep(n, step, 10, 5)
    experiments['n_scaling_4_2'] = static_n_sweep(n, step, 4, 2)
    experiments['n_scaling_9_3'] = static_n_sweep(n, step, 9, 3)
    experiments['n_scaling_5_2'] = static_n_sweep(n, step, 5, 2)
    experiments['n_scaling_3_1'] = static_n_sweep(n, step, 3, 1)
    experiments['n_scaling_1_1'] = static_n_sweep(n, step, 1, 1)

plot_all_n_scale(40_000, 4_000)
