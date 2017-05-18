# coding: utf-8
i = usbtmc.Instrument(0x1ab1, 0x04ce)

results = []
for f in np.linspace(10,20,21):
  my_cmd = "/home/brandon/asd 'f,{}'".format(f)
  args = shlex.split(my_cmd)
  subprocess.run(args)
  time.sleep(0.1)
  for v in np.linspace(0,10,21):
    my_cmd = "/home/brandon/asd 'v,{}'".format(v)
    args = shlex.split(my_cmd)
    subprocess.run(args)
    time.sleep(0.15)
    
    vapp = float(i.ask("MEASURE:VPP? CHAN1"))
    time.sleep(0.15)
    vapp += float(i.ask("MEASURE:VPP? CHAN1"))
    time.sleep(0.15)
    vapp += float(i.ask("MEASURE:VPP? CHAN1"))
    time.sleep(0.15)
    
    vamp = float(i.ask("MEASURE:VPP? CHAN3"))
    time.sleep(0.15)
    vamp += float(i.ask("MEASURE:VPP? CHAN3"))
    time.sleep(0.15)
    vamp += float(i.ask("MEASURE:VPP? CHAN3"))
    time.sleep(0.15)
    
    results.append((v,f,vamp/3,vapp/3))
    time.sleep(0.15)

np.savetxt('results4.csv',results,delimiter=',',fmt='%s')


data = results
fig = plt.figure()
ax = fig.add_subplot('111')
for i in range(0,10):
  # x: input voltage
  # y: output voltage (vamp, vapp)
  ax.plot([line[0] for line in data[i*21:(i+1)*21]], [line[2] for line in data[i*21:(i+1)*21]],color=(i*1/10,0,0))
  ax.plot([line[0] for line in data[i*21:(i+1)*21]], [line[3]/1000 for line in data[i*21:(i+1)*21]],color=(0,0,i*1/10))

ax.set_xlabel("input voltage")
ax.set_ylabel("measured voltage")
ax.grid()
fig.savefig('vv.png',dpi=300)
#!eom vv.png

fig = plt.figure()
ax = fig.add_subplot('111')
for i in range(0,21):
  # x: input voltage
  # y: output voltage (vamp, vapp)
  ax.plot([line[1] for line in data[i::21]], [line[2] for line in data[i::21]],color=(i*1/22,0,0))
  ax.plot([line[1] for line in data[i::21]], [line[3]/1000 for line in data[i::21]],color=(0,0,i*1/22))

ax.set_xlabel("input frequency")
ax.set_ylabel("measured voltage")
ax.grid()
fig.savefig('vf.png',dpi=300)
#!eom vf.png
