"""
    Contains all code regarding the CLI utility of the test engine.
    Contains all code regarding db config file parsing.
"""

import time
import argparse
import yaml
from configparser import ConfigParser

from DataTypes import DataType
from main import ExperimentRunner, ExperimentSettings

def load_config(filename='database.ini', section='postgresql'):
    parser = ConfigParser()
    parser.read(filename)

    config = {} 
    if parser.has_section(section):
        params = parser.items(section)
        for param in params:
            config[param[0]] = param[1]
    else:
        raise Exception(f'Section {section} not found in the {filename} file')
    return config

def positive_int(value):
        x = int(value)
        if x < 0:
            raise argparse.ArgumentTypeError("Must be positive")
        return x

def parse_args():
    '''
        creates and parses all the cli flags avaliable
    '''
    parser = argparse.ArgumentParser(
        description="AUDB Extension Experiment Runner", )

    exp_group = parser.add_mutually_exclusive_group(required=True)
    exp_group.add_argument(
        '--experiments-file',
        type=str,
        help="YAML file with defined experiments"
    )
    exp_group.add_argument(
        '--quick',
        action='store_true',
        help="Quick run experiment fully defined with CLI flags"
    )

    quick_group = parser.add_argument_group("Quick experiment options (must use --quick)")
    # experiment settings
    quick_group.add_argument(
        '-dt', '--data-type',
        choices=[ 'r', 's', 'range', 'set'],
        default='range',
        help='Data Type: range or set (default=range == r)'
    )
    quick_group.add_argument(
        '-nt', '--num-trials',
        type=positive_int,
        default=4,
        help='Number of trials (default=4)'
    )
    quick_group.add_argument(
        '-sz', '--dataset-size',
        type=positive_int,
        default=100,
        help='Dataset size/ Number rows. (default=100)'
    )
    quick_group.add_argument(
        '-ur', '--uncertainty-ratio',
        type=float,
        default=0.30,
        help='Uncertainty Ratio 0.0 - 1.0 (default=0.3)'
    )
    quick_group.add_argument(
        '-ni', '--num-intervals',
        type=positive_int,
        required=False,
        help='Fixed number of intervals in each Set'
    )
    quick_group.add_argument(
        '-gs', '--gap-size',
        type=positive_int,
        required=False,
        help='Fixed gap size between intervals'
    )
    quick_group.add_argument(
        '-nir', '--num-intervals-range',
        required=False,
        type=positive_int,
        nargs=2,
        help='Bounds for possible number of intervals in each Set. Ex: -nir lb ub'
    )
    quick_group.add_argument(
        '-gsr', '--gap-size-range',
        required=False,
        type=positive_int,
        nargs=2,
        help='Bounds for possible gap size between intervals Set. Ex: -gsr lb ub'
    )
    quick_group.add_argument(
        '-msr', '--mult-size-range',
        required=False,
        type=positive_int,
        nargs=2,
        help='Bounds for possible multiplicity range. Ex: -msr lb ub'
    )
    quick_group.add_argument(
        '-isr', '--interval-size-range',
        required=False,
        type=positive_int,
        nargs=2,
        help='Bounds for possible interval size. Ex: -isr a b'
    )
    
    # output options
    quick_group.add_argument(
        '-csv', '--save_csv',
        type=str,
        default='data',
        help='Directory for output files (default: data/)'
    )

    quick_group.add_argument(
        '-ddl', '--save_ddl',
        type=str,
        default='data',
        help='Directory for DDL code (default: data/)'
    )

    # database options
    quick_group.add_argument(
        '-dbc', '--dbconfig',
        type=str,
        default='database.ini',
        help='Database configuration file. (*.ini) (Default=database.ini)'
    )
    quick_group.add_argument(
        '-cb', '--clean-before',
        nargs='?',
        const='t_%',  
        default=None,
        help="Clean existing tables before running. Usage: --clean-before [PATTERN] (Default pattern: t_*)"
    )
    quick_group.add_argument(
        '-ca', '--clean-after',
        nargs='?',
        const='t_%',
        default=None,
        help="Clean existing tables after running. Usage: --clean-after [PATTERN] (Default pattern: t_*)"
    )
    
    # general settings
    quick_group.add_argument(
        '-m', '--mode',
        nargs='?',
        choices=[ 'generate-data', 'run-tests', 'all' ],
        default='all',
        help='Select what and what not to run. Usage: --mode [generate-data run-tests...] (Default=all)'
    )
    quick_group.add_argument(
        '-q', '--quiet',
        action='store_true',
        default='False',
        help='Quiet mode. Minimal Console output'
    )
    quick_group.add_argument(
        '-s', '--seed',
        type=int,
        default=None,
        help='Seed used to generate pseudo-randomness.'
    )


    return parser.parse_args()


def create_quick_experiment(args: argparse.Namespace) -> ExperimentSettings:
    '''
        parses the arguments into a ExperimentSettings object
    '''
    name = f"quick_{time.strftime('%Y%m%d_%H%M%S')}"

    return ExperimentSettings(
        name=name,
        data_type= DataType.RANGE if args.data_type=='range' or 'r' else DataType.SET,
        num_trials=args.num_trials,
        dataset_size=args.dataset_size,
        uncertain_ratio=args.uncertainty_ratio,
        num_intervals=args.num_intervals,
        gap_size=args.gap_size,
        num_intervals_range=args.num_intervals_range,
        gap_size_range=args.gap_size_range,
        mult_size_range=args.mult_size_range,
        interval_size_range=args.interval_size_range,
        make_csv=args.save_csv,
        mode=args.mode
    )


def load_experiments_from_file(args: argparse.Namespace) -> ExperimentSettings:
    with open()    

