%% Data reading in MATLAB written for bcduino
% for v?.? 
% place in folder in\temperature
disp('Accessing temperaturehistory file...')
fileID = fopen('temperaturehistory','r');

line=fgetl(fileID);
lskip=0;
while isempty((strfind(line,'2016')))   
   line=fgetl(fileID);
   lskip=lskip+1;
end

garbage=fgetl(fileID);%read and discard first two lines
garbage=fgetl(fileID);

%file has form %timestamp /milisecond counter/
%V measured/flowrate q mmasured/ frequency omega measured/
% V set/ q set/ omeg
% tube temperature/ surface temperature

t_stamp=fgetl(fileID); %get first line of experimental data
fclose(fileID); %close file

t_stamp=t_stamp(1:19); % get the first timestamp
fprintf('Experiment start time %s\n',t_stamp);
formatIn=('yyyy-mm-dd_HH:MM:SS'); %set date vector format
t_ref=datevec(t_stamp,formatIn); %get the starting point

A=csvread('temperaturehistory',lskip,1);

milisecondcounter=A(:,1)/1000; %in seconds
disp('Calculating time vector...')
seconds=sum((-diff(milisecondcounter)>0.5));

t=[];
sec_counter=0;
sampler=-diff(milisecondcounter)>0.5; %figure out when a secodn passes
t=1:1:sum(sampler); %create time vector in seconds


Vm=[0;sampler].*A(:,2); 
Vm=Vm(Vm~=0); %sample from measured voltage

data2=[0;sampler].*A(:,3); 
data2=data2(data2~=0); %sample from voltage

data3=[0;sampler].*A(:,4); 
data3=data3(data3~=0); %sample from voltage

Vset=[0;sampler].*A(:,5); 
Vset=Vset(Vset~=0)*10; %sample from set voltage

D5=A(:,6);
D5(D5==0)=0.0001;
data5=[0;sampler].*D5; 
data5=data5(data5~=0); %sample from voltage

D6=A(:,7);
D6(D6==0)=0.01;
data6=[0;sampler].*D6; 
data6=data6(data6~=0); %sample from voltage


Tt=[0;sampler].*A(:,8); 
Tt=Tt(Tt~=0); %sample from voltage

Ts=[0;sampler].*A(:,9); 
Ts=Ts(Ts~=0); %sample from voltage

% figure
% subplot(3,1,1)
% plot(t,Vm,t,Vset,'linewidth',2)

disp('Accessing oscilloscope data...');

cd ..
cd('./oscilloscope')
B=ls; %read the list of files
formatIn=('yyyy-mm-dd_HH_MM_SS'); %set date vector format

VPI=[];
osc_time=[];
P_lisj=[];
phase=[];
P_rms=[];
fr=[];
C=47e-9; %F lissijious capacitance
for k=3:1:length(B)-1
    t1=B(k,1:19); 
    t1=datevec(t1,formatIn);
    vi=csvread(B(k,:));
    
    V=vi(:,2);
    Vcap=vi(:,3);
    I=vi(:,4);
  
    
    fit_obj=fit(vi(:,1),V,'sin1');
    freq=fit_obj.b1/(2*pi);
    
    fit_obj2=fit(vi(:,1),I,'sin1');
    ph=fit_obj2.c1;
    
    if freq>1e3;
        ts=abs(vi(1,1)-vi(2,1));
        ind=1/freq/ts;
        
        f=vi(1,1)-vi(end,1)/freq;

         P_lis=C*freq*polyarea(V(1:round(ind)),Vcap(1:round(ind)));

        VPI=[VPI; rms(V) rms(Vcap) rms(I)];
        P_rms=[P_rms; rms(V.*I)];
        P_lisj=[P_lisj; P_lis];
        fr=[fr; freq];
        phase=[phase; ph];

        osc_time=[osc_time; etime(t1,t_ref)];
    end
end


%create saving variable
expr.Vrms=VPI(:,1);
expr.CapVrms=VPI(:,2);
expr.irms=VPI(:,3);
expr.t_osc=osc_time;
expr.t_temp=t;
expr.raw_tempdata=A;
expr.Vm=Vm;
expr.data2=data2;
expr.data3=data3;
expr.Vset=Vset;
expr.data5=data5;
expr.data6=data6;
expr.T_tube=Tt;
expr.T_surface=Ts;
expr.Prms=P_rms;
expr.freq=fr;
expr.P_lisj=P_lisj;
cd ..
save('data_extract.mat','-struct','expr')