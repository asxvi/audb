from cliUtility import *
from DataTypes import *
from dataclasses import replace
from main import format_name, ExperimentGroup

'''
experiments is a dict of {str: ExperimentGroup}. ALlows for many unrelated experiments to run from 1 file
Naming convention is "GroupName/ID": {ExperimentGroup of related experiments}
persists in namespace of caller program
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

dataset_size_group1 = ExperimentGroup('dataset_size_experiments1', 'dataset_size', None)
for i in range(100, 1000, 100):
    experiment = replace(
            template,
            dataset_size = i, 
            independent_variable = 'dataset_size',
        )
    experiment.name = format_name(experiment)       # name after setting members
    
    dataset_size_group1.experiments[experiment.name] = experiment

experiments['dataset_size1'] = dataset_size_group1

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

'''
    add more code in similar format as above. respect naming convention please!
    ...
    
'''