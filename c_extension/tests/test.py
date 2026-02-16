from cliUtility import *
from DataTypes import *
from main import ExperimentRunner, generate_seed


def getStuff(self):
    table = 't_s_n10_unc00_ni2_gsr2500_40000_redsz2_1__iv_red_sz_t1_ms174377'
    config = self.DATA_TYPE_CONFIG[DataType.SET]

    results = {
        'row_count' : 0,
        'min_time' : None,
        'max_time' : None,
        'sum_time' : None,
        'sumtest_time': None,
        # 'min_result' : None,
        # 'max_result' : None,
        # 'sum_result' : None,

        'sum_test_result' : None,
        'reduce_calls' : None,
        'max_interval_count': None,
        'total_interval_count': None,
        'combine_calls': None,
        'result_size': None,

        'accuracy_size_ratio': None,
        'accuracy_coverage_ratio': None,
        'accuracy_jaccard': None,
    }

    with self.connect_db() as conn:
        with conn.cursor() as cur:  
            # get additional tests for sumtest
            normalize = True
            int_max = 2147483647
            # results['sumtest_time'] = self.run_aggregate(cur, table, 'SUMTEST', config['combine_sum'], experiment.reduce_triggerSz_sizeLim[0], experiment.reduce_triggerSz_sizeLim[1], normalize)
            metrics = self.get_sumtest_metrics(cur, table, config['combine_sum'], 10, 2, normalize)
            ground_truth_metrics = self.get_sumtest_metrics(cur, table, config['combine_sum'], int_max, int_max, normalize)
            
            accuracy_metrics = self.calculate_accuracy(metrics, ground_truth_metrics)

            if metrics: 
                results['sum_test_result'] = metrics['result']
                results['reduce_calls'] = metrics['reduce_calls']
                results['max_interval_count'] = metrics['max_interval_count']
                results['total_interval_count'] = metrics['total_interval_count']
                results['combine_calls'] = metrics['combine_calls']
                results['result_size'] = metrics['result_size']
            if accuracy_metrics:
                results['accuracy_coverage_ratio'] = accuracy_metrics['cover_accuracy']
                results['accuracy_size_ratio'] = accuracy_metrics['size_accuracy']
                results['accuracy_jaccard'] = accuracy_metrics['jaccard_index']
    
    print(results)


if __name__ == '__main__':

    master_seed = generate_seed(1743778984)

    print(master_seed)

    print("The unique seed is ", master_seed)
    db_config = load_config()    

    runner = ExperimentRunner(db_config, master_seed)

    getStuff(runner)