import numpy as np
import matplotlib.pyplot as plt


def get_n(nb_name, ns_name):
    global Nb, Ns
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

    Nb = np.array(Nb)
    Ns = np.array(Ns)

    return Nb, Ns


def get_name(full_file_name):
    start_index = full_file_name.find('_') + 1  # Find the index of the first '_' character
    end_index = full_file_name.find('.')  # Find the index of the first '.' character

    if start_index != -1 and end_index != -1:
        return full_file_name[start_index:end_index]  # Extract substring between '_' and '.'
    else:
        return "Characters not found"


def plot_cut_function(cut, nb_name, ns_name):
    nb, ns = get_n(nb_name, ns_name)

    # Get the name of the feature:
    name = get_name(nb_name)

    # Calculate function
    func = ns / np.sqrt(ns + nb)

    nan_pos = np.isnan(func)
    func[nan_pos] = 0

    # Find peak
    peak_ind = np.argmax(func)
    print("The peak is at %.2f" % cut[peak_ind])

    # Plot
    plt.plot(cut, func)
    plt.title(f'Ratio of Cuts for {name}')
    plt.xlabel('Cut')
    plt.ylabel('f(cut)')
    plt.savefig(f'full_set_cut_plots/cut_functions/{name}')
    plt.show()

    return nb, ns, name


def plot_histogram(cuts, N, name, feature_type):

    plt.hist(cuts, cuts, weights=N, align='left')
    plt.xlabel('Cut')

    if feature_type:
        plt.title(f'Cuts for Signal {name}')
        plt.ylabel('Ns')
        plt.savefig(f'full_set_cut_plots/signal_hist/{name}')
    else:
        plt.title(f'Cuts for Background {name}')
        plt.ylabel('Nb')
        plt.savefig(f'full_set_cut_plots/background_hist/{name}')
    plt.show()


cuts = np.arange(0, 102, 2)

nb, ns, name = plot_cut_function(cuts, nb_name='data/background_nconst1.csv',
                                 ns_name='data/signal_nconst1_1.csv')

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
