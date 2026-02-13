from cliUtility import *
from DataTypes import *
from numerize import numerize
from dataclasses import replace
from main import format_datasize, format_name, ExperimentGroup



'''
experiments is a dict of {str: dict}. ALlows for many unrelated experiments to run from 1 file
Naming convention is "independentVariable member": {experiments of type ExperimentSettings}
persists in namespace of caller program

Structure: {
    "experiment_group_name": {
        "independent_variable": "variable_name",  # specify the x-axis variable
        "experiments": {
            "exp1_name": ExperimentSettings(...),
            "exp2_name": ExperimentSettings(...),
        }
    }
}

'''
experiments = dict()

# dummy used to access members
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
        name= "temp",)


dataset_size_group = ExperimentGroup('dataset_size_experiments', 'dataset_size', None)
for i in range(100, 1000, 100):
    experiment = replace(
            template,
            dataset_size = i, 
            independent_variable = 'dataset_size',
        )
    experiment.name = format_name(experiment) + f'__{i}'
    
    dataset_size_group.experiments[experiment.name] = experiment

experiments['dataset_size'] = dataset_size_group


dataset_size_group2 = ExperimentGroup('dataset_size_experiments2', 'dataset_size', None)
for i in range(100, 1000, 100):
    experiment = replace(
            template,
            dataset_size = i, 
            independent_variable = 'dataset_size',
        )
    experiment.name = format_name(experiment)
    
    dataset_size_group2.experiments[experiment.name] = experiment

experiments['dataset_size2'] = dataset_size_group2

# for i in range(0, 1000, 100):
#     experiment = replace(
#             template,
#             dataset_size = i, 
#             independent_variable = 'dataset_size',
#         )
#     experiment.name = format_name(experiment) + f'__{i}'

#     dataset_size_experiments[experiment.name] = experiment
# experiments['dataset_size'] = dataset_size_experiments

# reduce_triggerSz_sizeLim_experiments = {}
# for i in range(1, 50):
#     experiment = replace(
#             template,
#             reduce_triggerSz_sizeLim = (i, max(1, i//2)),
#         )
#     name = format_name(template) + f'{i}'
#     reduce_triggerSz_sizeLim_experiments[experiment.name] = experiment
# experiments['reduce_triggerSz_sizeLim'] = dataset_size_experiments