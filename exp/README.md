## Experimental Runtime

This folder will contain the newest version of the runtime scripts.

It is guaranteed to, at the very minimum, run.

Folders:

+ scripts - contains the various runtime scripts
+ data - contains the experimental data


## Status

+ oscilloscope.py
  + TESTED
  + environment: `workon science`
  + occasionally crashed by uncaught hardware read errors
  + takes cmdline arguments; uses argparse to auto-generate help
  + saves timestamped csv files in specified folder in a loop
+ spectroscopy.py
  + TESTED
  + environment: `workon science`
  + scales integration time to keep maximum measured values within a specified range
  + cmdline arguments; uses argparse to auto-generate help
  + saves timestamped csv files in specified folder in a loop
  + filename has the integration time appended for normalization
+ thermography.py
  + 




## Software

### Installed System-Wide

libusb-dev python-opencv

### Python Environment

`workon science`

`pip freeze`

    backports-abc==0.4
    backports.ssl-match-hostname==3.5.0.1
    certifi==2016.2.28
    cffi==1.5.2
    cryptography==1.3.1
    cycler==0.10.0
    decorator==4.0.9
    enum34==1.1.2
    functools32==3.2.3.post2
    futures==3.0.5
    idna==2.1
    ipaddress==1.0.16
    ipykernel==4.3.1
    ipyparallel==5.0.1
    ipython==4.1.2
    ipython-genutils==0.1.0
    ipywidgets==4.1.1
    Jinja2==2.8
    jsonschema==2.5.1
    jupyter==1.0.0
    jupyter-client==4.2.2
    jupyter-console==4.1.1
    jupyter-core==4.1.0
    MarkupSafe==0.23
    matplotlib==1.5.1
    mistune==0.7.2
    nbconvert==4.1.0
    nbformat==4.0.1
    ndg-httpsclient==0.4.0
    notebook==4.1.0
    numpy==1.11.0
    path.py==8.1.2
    pexpect==4.0.1
    pickleshare==0.6
    ptyprocess==0.5.1
    pyasn1==0.1.9
    pycparser==2.14
    Pygments==2.1.3
    pyOpenSSL==16.0.0
    pyparsing==2.1.1
    pyserial==3.0.1
    python-dateutil==2.5.2
    python-usbtmc==0.8
    pytz==2016.3
    pyusb==1.0.0
    PyVISA==1.8
    PyVISA-py==0.2
    pyzmq==15.2.0
    qtconsole==4.2.1
    requests==2.9.1
    RPi.GPIO==0.6.2
    sh==1.11
    simplegeneric==0.8.1
    singledispatch==3.4.0.3
    six==1.10.0
    terminado==0.6
    tornado==4.3
    traitlets==4.2.1

