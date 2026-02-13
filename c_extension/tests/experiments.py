from cliUtility import *
from DataTypes import *
from numerize import numerize
from dataclasses import replace

'''
# ADD TESTS HERE

# SAMPLE CODE
# trial1 = ExperimentSettings(name="test1", data_type=DataType.RANGE, dataset_size=10, uncertain_ratio=0.1, mult_size_range=(1,5),
#                             interval_size_range=(1, 100), num_intervals=2, num_intervals_range=(1,3), save_csv=False, save_ddl=True, mode="all",
#                             num_trials=2, gap_size_range=(0,5), gap_size=None)
# runner.run_experiment(trial1)
# sample_sizes = [100, 1000, 5000, 10000, 25000, 50000]
# for s in sample_sizes:
#     trial = ExperimentSettings(name=f"test_{s}", data_type=DataType.RANGE, dataset_size=s, uncertain_ratio=0.1, mult_size_range=(1,5),
#                             interval_size_range=(1, 100), save_csv=False, save_ddl=True, mode="all",
#                             num_trials=2, gap_size_range=(0,5), gap_size=None)
#     runner.run_experiment(trial)

# runner.clean_tables(db_config)
'''


def format_datasize(size):
        if size >= 1_000_000: 
            return str.replace(numerize.numerize(size, 2), '.', '_')
        return numerize.numerize(size, 0)

def format_name(experiment: ExperimentSettings):
    dtype = 's' if experiment.data_type == DataType.SET else 'r'
    sz = format_datasize(experiment.dataset_size)
    unc = str.replace(str(experiment.uncertain_ratio), '.', '_')

    return f"{dtype}_n{sz}_unc{unc}"

experiments = {}

template = ExperimentSettings(
        data_type=DataType.SET, 
        dataset_size=100_000, 
        uncertain_ratio=0.1, 
        mult_size_range=(1,5),
        interval_size_range=(1, 500), 
        num_intervals=4, 
        mode="all",
        num_trials=3, 
        gap_size_range=(0,5), 
        gap_size=10,
        name= "temp", 
    )

for i in range(0, 100000, 10000):
    experiment = replace(
        template,
        # reduce_triggerSz_sizeLim = (i, max(1, i//2)),
        dataset_size = i, 
        name = format_name(experiment) + f'__{i}',
    )
    experiments[experiment.name] = experiment

# for i in range(1, 50):
#     experiment = replace(
#         template,
#         name = format_name(template) + f'{i}',
#         reduce_triggerSz_sizeLim = (i, max(1, i//2)),
#     )
    
