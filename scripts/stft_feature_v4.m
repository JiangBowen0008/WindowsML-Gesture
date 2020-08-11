%%
clear all;
close all
fs1=96e3;
fs2=48e3;
fs=fs2;
fc=19e3;
nfft=8192;
step=2048;
f_resolution=fs/nfft;
c_point=round(fc/f_resolution)+1;
N=floor(400/f_resolution);
alpha=0.2;
F=(0:nfft-1)*(fs/nfft);
f=F(c_point-N+1:c_point+N);

window=hamming(nfft);
% window=repmat(window,[1,2]);
signal=zeros(8192,2);
tf_gest_ch1=zeros(72,122);
tf_gest_ch2=zeros(72,122);
mySignal=zeros(2048,2);
rms_debug=zeros(1,150);
rms_wb_debug=zeros(1,150);
% mySignal= gpuArray(mySignal);
% signal = gpuArray(signal);
% rms_debug = gpuArray(rms_debug);
% rms_wb_debug = gpuArray(rms_wb_debug);
% tf = gpuArray(tf);
% window=gpuArray(window);
vad_tf=rand(14,60);
% vad_tf=gpuArray(vad_tf);
vad_debug_plot=zeros(1,72);
% vad_debug_plot = gpuArray(vad_debug_plot);


% load('vad_net_sl_60_nh_64.mat')
% load('gest_net6.mat')
% gest_net=net;
%%




filename_list=dir(['.\data/*.mat']);
% filename_list=dir(['D:\matlab code\联想手势\x390\data\origin\yjb/*.mat']);
% filename_list=dir(['./noise0622/*.wav']);
% filename_list=dir(['D:\matlab code\联想手势\笔记本自带麦克风\0506\chenzy\1/*.wav']);
flag_noise=1;
for idx_file=1:12%:length(filename_list)
    idx_gest=0;
%     idx_file=1;
    filename=fullfile(filename_list(idx_file).folder,filename_list(idx_file).name);
    if filename_list(idx_file).name(end-2:end)=='mat'
        load(filename)
    else
    [data,fs]=audioread( filename);
    end


    %     idx_frame=1;
    
    %
    Ep_front_ch1=zeros(1,length(f));
    start_flag=0;
    end_flag=0;
    gest_count=0;
    lost_count=0;
    start_count=0;
    
   
    debug=2;
    SNR=5;
    [data,str_fn]=add_noise(data,debug,SNR);
   
    
    ch1=data(:,1);
    ch2=data(:,2);
    
    [tf_ch1,~,~,~]=spectrogram(ch1,window,nfft-step,nfft);
    [tf_ch2,~,~,~]=spectrogram(ch2,window,nfft-step,nfft);
    
   
       
    
    sp=tf_ch1(c_point-N:c_point+N-1,1);
    Ep=sp.*conj(sp)/(0.5*sum(window)^2);
    Ep_front_ch1=Ep;
    
    sp=tf_ch2(c_point-N:c_point+N-1,1);
    Ep=sp.*conj(sp)/(0.5*sum(window)^2);
    Ep_front_ch2=Ep;
    
    for idx_frame=1:size(tf_ch1,2)
        
        Spect_ch1=tf_ch1(:,idx_frame);
        Spect_ch2=tf_ch2(:,idx_frame);
        tf_gest_ch1(1:end-1,:)=tf_gest_ch1(2:end,:);%137*70*2
        tf_gest_ch2(1:end-1,:)=tf_gest_ch2(2:end,:);
        [sp2_ch1,feature_ch1, Ep_front_ch1]=exact_feature(Spect_ch1,c_point,N,window,f, Ep_front_ch1);
        [sp2_ch2,feature_ch2, Ep_front_ch2]=exact_feature(Spect_ch2,c_point,N,window,f, Ep_front_ch2);
        tf_ch1_norm(:,idx_frame)=sp2_ch1/max(abs(Spect_ch1));
        tf_ch1_temp(:,idx_frame)=sp2_ch1;
        feature(:,idx_frame)=feature_ch1;
       
        
    end
    feature2=exact_feature_matlab(ch1,fs);
    
    
    Y=zeros(1,size(feature,2));
    if flag_noise
%         filename_vad=fullfile('./vadpoint',['vadpoint_',filename_list(idx_file).name(1:end-3),'mat']);
%         load(filename_vad)
        load('.\vad_point.mat');
        start_point=vad_point(:,2*idx_file-1);
        end_point=vad_point(:,2*idx_file);
        for i=1:length(start_point)
            Y(start_point(i):end_point(i))=1;
        end
        
       
    end
     vad=categorical(Y);
     
    tf_ch1=tf_ch1_temp;
    gest_mat_fn=['.\feature0729\',str_fn,filename_list(idx_file).name(1:end-4),'_',num2str(idx_gest),'.mat'];
    save(gest_mat_fn,'feature','tf_ch1','tf_ch1_norm','vad');
end


%% add_noise
function [data,str_fn]=add_noise(data,debug_noise,SNR)
if debug_noise==1
    %% addnoise
    noise=randn(1,length(data))';
%     SNR                 = 10;
    noise               = 10^(-SNR/20) * noise * norm(data(:,1)) / norm(noise);
    ch1_Noisy  =data(:,1) + noise;
    %     ch1_Noisy  = ch1_Noisy / max(abs(ch1_Noisy));
    data(:,1)=ch1_Noisy;
    
    noise=randn(1,length(data))';
%     SNR                 = 10;
    noise               = 10^(-SNR/20) * noise * norm(data()) / norm(noise);
    ch2_Noisy  =data(:,2) + noise;
    %     ch1_Noisy  = ch1_Noisy / max(abs(ch1_Noisy));
    data(:,2)=ch2_Noisy;
    str_fn=['addnoise_',num2str(SNR),'dB_'];
    
    
    
elseif debug_noise==2
    %% add speech
    datafolder ='.\《宋词三百首》有声朗读版\《宋词三百首》朗读版';% PathToDatabase;
    ads        = audioDatastore(datafolder,"Includesubfolders",true);
    ads=shuffle(ads);
    [data_music,info] = read(ads);
    data_music=data_music(:,1);
    fs2 = info.SampleRate;
    segments   = HelperGetSpeechSegments(data_music,fs2);
    speechData = segments{1};
    duration      = 200*fs2;
    audiodata = zeros(duration,1);
    numSamples = 1;
    while numSamples < duration
        data_music = read(ads);
        % Remove non-voice areas from the segment
        %         data = HelperGetSpeechSegments(data(:,1),fs2);
        %         for idx=1:length(data)
        temp= data_music(:,1);
        % Write speech segment to training signal
        audiodata(numSamples:numSamples+numel(temp)-1) =temp;
        numSamples        = numSamples + numel(temp) ;
    end
    fs=48e3;
    noise=audiodata(((randi(9)-1)*fs)+(1:length(data)));
    audiodata=resample(audiodata(:,1),fs,fs2);
    [b,a]=butter(4,15e3/(fs/2),'high');
    noise=filter(b,a,audiodata);
    noise=noise(1:length(data));
    %     SNR                 = 0;
    noise               = 10^(-SNR/20) * noise * norm(data) / norm(noise);
    ch1_Noisy  = data (:,1)+ noise;
    data(:,1)=ch1_Noisy;
     ch2_Noisy  = data (:,2)+ noise;
    data(:,2)=ch2_Noisy;
    str_fn=['addmusic_',num2str(SNR),'dB_'];
%         save_fn= fullfile('E:\x390\feature2\',['addmusic_',num2str(SNR),'dB_',filename_list(idx_file).name]);
elseif debug_noise==3
        noise=randn(1,length(data))';
%     SNR                 = 10;
    noise               = 10^(-SNR/20) * noise * norm(data(:,1)) / norm(noise);
    ch1_Noisy  =data(:,1) + noise;
    %     ch1_Noisy  = ch1_Noisy / max(abs(ch1_Noisy));
    data(:,1)=ch1_Noisy;
    
    noise=randn(1,length(data))';
%     SNR                 = 10;
    noise               = 10^(-SNR/20) * noise * norm(data()) / norm(noise);
%     ch2_Noisy  =data(:,2) + noise;
    %     ch1_Noisy  = ch1_Noisy / max(abs(ch1_Noisy));
    data(:,2)=noise ;
    str_fn=['noise_',num2str(SNR),'dB_'];
elseif debug_noise==4
   %% add speech
    datafolder ='F:\BaiduNetdiskDownload\《宋词三百首》有声朗读版\《宋词三百首》朗读版';% PathToDatabase;
    ads        = audioDatastore(datafolder,"Includesubfolders",true);
    ads=shuffle(ads);
    [data_music,info] = read(ads);
    data_music=data_music(:,1);
    fs2 = info.SampleRate;
    segments   = HelperGetSpeechSegments(data_music,fs2);
    speechData = segments{1};
    duration      = 200*fs2;
    audiodata = zeros(duration,1);
    numSamples = 1;
    while numSamples < duration
        data_music = read(ads);
        % Remove non-voice areas from the segment
        %         data = HelperGetSpeechSegments(data(:,1),fs2);
        %         for idx=1:length(data)
        temp= data_music(:,1);
        % Write speech segment to training signal
        audiodata(numSamples:numSamples+numel(temp)-1) =temp;
        numSamples        = numSamples + numel(temp) ;
    end
    fs=48e3;
    noise=audiodata(((randi(9)-1)*fs)+(1:length(data)));
    audiodata=resample(audiodata(:,1),fs,fs2);
    [b,a]=butter(4,15e3/(fs/2),'high');
    noise=filter(b,a,audiodata);
    noise=noise(1:length(data));
    %     SNR                 = 0;
    noise               = 10^(-SNR/20) * noise * norm(data) / norm(noise);
    ch1_Noisy  = data (:,1)+ noise;
    data(:,1)=ch1_Noisy;
%      ch2_Noisy  = data (:,2)+ noise;
    data(:,2)=noise;%ch2_Noisy;
    str_fn=['music_',num2str(SNR),'dB_'];
%         save_fn= fullfile('E:\x390\feature2\',['addmusic_',num2str(SNR),'dB_',filename_list(idx_file).name]);  
else
    data=data;
    %         save_fn= fullfile('E:\x390\feature2',[filename_list(idx_file).name]);
    str_fn=[];
end
end
%% exact_feature
function [sp2,feature,Ep_front]=exact_feature(Spect,c_point,N,window,f,Ep_front)
sp=Spect(c_point-N:c_point+N-1)';
Ep=sp.*conj(sp)/(0.5*sum(window)^2);%能量

Ep_m=abs(sp)/(0.5*sum(window));%幅度

prob=Ep/sum(Ep);

%%
sp2=abs([Spect(c_point-N:c_point-8);Spect(c_point+8:c_point+N)]);
Ep2=sp2.*sp2;
rms=sum(Ep2);




sentroy=-sum(prob.*log2(prob+eps))./log2(length(prob));%entroy
scentroid=sum(f.*Ep)/sum(Ep);%centroid
screst=max(Ep)/(sum( Ep)/length(Ep));
sflux=norm(Ep- Ep_front,2);%flux
Ep_front=Ep;
sspread=sqrt(sum((f-scentroid).^2.*Ep)./sum(Ep));
skurtosis=sum((f-scentroid).^4.*Ep)./( sspread.^4.*sum(Ep));
sskewness=sum((f-scentroid).^3.*Ep)/ (sspread.^3*sum(Ep));
sslope=sum((f-mean(f)).*(Ep_m-mean(Ep_m)))./sum((f-mean(f)).^2);
sflat=exp(sum(log(Ep + eps(class(gather(Ep)))))/length(Ep))./(sum(Ep/length(Ep)));%prod(Ep).^(1/length(Ep))/(sum(Ep)/length(Ep));
sdecrease=sum((Ep(2:end)-Ep(1))./(1:length(Ep)-1))/sum(Ep(2:end));
c = cumsum(Ep2);
d = c(end)*0.7;
idx = find(c>= d,1);
srolloffPoint = f(idx);
[s_max,s_loc]=max(sp2);
feature=[rms;sentroy;scentroid;screst;sflux;skurtosis; sspread;sskewness;sslope; sflat; sdecrease;s_max;s_loc];
end

%% exact_feature2
function feature2=exact_feature_matlab(ch1,fs)
  SEntropy = spectralEntropy(ch1,fs, ...
        'Window',hamming(8192), ...
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    SCentroid     = spectralCentroid(ch1,fs, ...
        'Window',hamming(8192), ...
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    
    SCrest        = spectralCrest(ch1,fs, ...
        'Window',hamming(8192), ...
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SFlux         = spectralFlux(ch1,fs, ...
        'Window',hamming(8192), ...
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SKurtosis     = spectralKurtosis(ch1,fs, ...
        'Window',hamming(8192), ...
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SRolloffPoint = spectralRolloffPoint(ch1,fs, ...
        'Window',hamming(8192), ...
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SSkewness     = spectralSkewness(ch1,fs, ...
        'Window',hamming(8192), ...
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SSlope        = spectralSlope(ch1,fs, ...
        'Window',hamming(8192), ...
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SSpread        = spectralSpread(ch1,fs, ...
        'Window',hamming(8192), ...
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SDecrease        = spectralDecrease(ch1,fs, ...
        'Window',hamming(8192), ...
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SFlatness      = spectralFlatness(ch1,fs, ...
        'Window',hamming(8192), ...
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
     feature2=[SEntropy';SCentroid';SCrest';SFlux';SKurtosis'; SSpread';SSkewness';SSlope'; SFlatness' ; SDecrease';SRolloffPoint'];
end


