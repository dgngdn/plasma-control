
## get the highest temperature in the most recent thermography data

a = os.listdir('data/thermography/'); a.sort(); data = np.genfromtxt('data/thermography/{}'.format(a[-1]), dtype=None, delimiter=','); data.max()

