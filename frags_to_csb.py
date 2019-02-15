"""
    Given a dataframe, sorted by diagonal and starting x coordinate, 
    returns a csv file containing the CSBs obtained from putting together 
    all suitable frags (i.e.: If, when two frags are put together, the
    new score is greater than 0)
"""

import pandas as pd
import matplotlib.pyplot as plt
import itertools
from common_functions import update_csb, is_suitable

def frags_to_csb (df, x_len, y_len):
    it_frags = df.iterrows()
    current_frag = next(it_frags)
    current_csb = current_frag
    dropped_frags = dict()

    df_csb = pd.DataFrame()
    try:
        while True:
            it_frags, it_candidates = itertools.tee(it_frags)
            next_candidate = next(it_candidates)
            continue_value = 1
            try:
                while continue_value > 0.1 :
                    if (is_suitable(current_frag[1], next_candidate[1])):
                        dropped_frags[next_candidate[0]] = True
                        current_csb = update_csb(current_csb, next_candidate[1], max(x_len, y_len))
                        continue_value = min(continue_value * 2, 1)
                    else:
                        continue_value = continue_value / 2
                    next_candidate = next(it_candidates)
            except StopIteration:
                pass
            finally:
                del it_candidates
            df_csb = df_csb.append(current_csb[1].to_frame().T)
            condition = True
            while condition:
                next_frag = next(it_frags)
                current_csb = next_frag
                current_frag = next_frag
                condition = next_frag[0] in dropped_frags
    except StopIteration:
        pass
    finally:
        df_csb = df_csb.append(current_csb[1].to_frame().T)
        del it_frags

    return df_csb
    
