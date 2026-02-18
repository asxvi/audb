# VIABLE_NI     = [3, 4, 5]
# TRIGGER_STEPS = [3, 6, 9, 12, 18, 24, 30]

# def candidate_size_lims(trigger, ni):
#     # size_lim must be >= ni to keep result interpretable (never collapse below natural floor)
#     candidates = {
#         ni,                          # natural floor — reduce to exactly k intervals
#         max(ni, trigger // 2),
#         max(ni, int(trigger * 0.75)),
#         trigger - 1,
#     }
#     return sorted(c for c in candidates if ni <= c < trigger)

# pareto_sweep = ExperimentGroup('pareto_reduce', 'reduce_triggerSz_sizeLim', None)

# for ni in VIABLE_NI:
#     for trigger_sz in TRIGGER_STEPS:
#         if trigger_sz <= ni:         # trigger must exceed ni or it fires immediately
#             continue
#         for size_lim in candidate_size_lims(trigger_sz, ni):
#             experiment = replace(
#                 template,
#                 dataset_size             = 100,
#                 num_trials               = 3,
#                 uncertain_ratio          = 0.0,
#                 independent_variable     = 'reduce_triggerSz_sizeLim',
#                 reduce_triggerSz_sizeLim = (trigger_sz, size_lim),
#                 interval_size_range      = (1, 2_000_000_000),
#                 start_interval_range     = (1, 1_000),
#                 gap_size_range           = (500_000, 2_000_000),
#                 interval_width_range     = (50, 200),
#                 num_intervals            = ni,
#             )
#             experiment.name = f"ni{ni}_redSz{trigger_sz}_{size_lim}"
#             pareto_sweep.experiments[experiment.name] = experiment

# experiments['pareto_reduce'] = pareto_sweep