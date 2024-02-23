import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import find_peaks


def get_n(nb_name, ns_name):

    # Read in data
    with open(nb_name, 'r') as data:
        Nb = []
        for line in data:
            row = line.split(',')
            Nb.append(float(row[0]))

    with open(ns_name, 'r') as data:
        Ns = []
        for line in data:
            row = line.split(',')
            Ns.append(float(row[0]))

    Nb = np.array(Nb) * 10 * 363          # Scaling by IntLumi and CrossSection
    Ns = np.array(Ns) * 10 * 7.38400e-5

    return Nb, Ns


def get_name(full_file_name):
    start_index = full_file_name.find('_') + 1  # Find the index of the first '_' character
    end_index = full_file_name.find('.')  # Find the index of the first '.' character

    if start_index != -1 and end_index != -1:
        return full_file_name[start_index:end_index]  # Extract substring between '_' and '.'
    else:
        return "Characters not found"


def plot_cut_function(cut, nb_name, ns_name):
    Nb, Ns = get_n(nb_name, ns_name)

    # Get the f_name of the feature:
    feature_name = get_name(nb_name)

    # Calculate function
    func = Ns / np.sqrt(Ns + Nb)

    nan_pos = np.isnan(func)
    func[nan_pos] = 0

    # Find peak
    peak_ind = np.argmax(func)
    print("The peak is at %.2f" % cut[peak_ind])

    # peaks = find_peaks(func)[0]
    # print(cut[peaks])

    # Plot
    plt.plot(cut, func)
    plt.title(f'Ratio of Cuts for {feature_name}')
    plt.xlabel('Cut')
    plt.ylabel('f(cut)')
    plt.savefig(f'correct_full_set_cuts/cut_sig/{feature_name}')
    plt.show()

    return Nb, Ns, feature_name


def plot_histogram(cut, N, f_name, feature_type):

    plt.hist(cut, cut, weights=N, align='left')
    plt.xlabel('Cut')

    if feature_type:
        plt.title(f'Cuts for Signal {f_name}')
        plt.ylabel('Ns')
        plt.savefig(f'correct_full_set_cuts/signal_h/{f_name}')
    else:
        plt.title(f'Cuts for Background {f_name}')
        plt.ylabel('Nb')
        plt.savefig(f'correct_full_set_cuts/background_h/{f_name}')
    plt.show()


cuts = np.arange(0, 1.02, 0.02)

nb, ns, name = plot_cut_function(cuts, nb_name='data/background_jj_gg1.csv',
                                 ns_name='data/signal_jj_gg1_1.csv')

plot_histogram(cuts, ns, name, 1)
plot_histogram(cuts, nb, name, 0)

'''Plotting histograms from raw data'''

# with open('data.csv', 'r') as data:
#     test_data = []
#     for line in data:
#         row = line.split(',')
#         test_data.append((row[9]))

# test_data = np.array(test_data)[1:]
# print(test_data)
# test_data = test_data.astype(float)
#
# plt.hist(test_data, bins=100)
# plt.show()

# test = 'data/signal_jj_e1_1.csv'
# res = get_name(test)
# print(res)
