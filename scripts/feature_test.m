%%
clear all;
close all

<<<<<<< HEAD
fileID = fopen("logAudio.txt");
signal = textscan(fileID, '%f %f');
ch1 = signal{1};
fclose(fileID)

s1 = textread("logfft.txt","%f");
s1_window = reshape(s1, [], 200);

f_complete = textread("log.txt","%f");
f_window = reshape(f_complete, [], 200);

% feat = extract_feature_matlab(ch, 48e3)

%%
myVoice = audiorecorder(48e3,24,1);
record(myVoice, 5);

% Define callbacks to show when
% recording starts and completes.
myVoice.StartFcn = 'disp(''Start speaking.'')';
myVoice.StopFcn = 'disp(''End of recording.'')';

test = getaudiodata(myVoice, 'float32');

%%
fs =48e3;
nfft = 8192;
step = 2048;
figure(1)
subplot(212)
window = hamming(8192);
% [~,~,datafft_,~] = fftwindow(ch1, 19e3);
datafft_ = spectrogram(ch1, window, nfft - step, nfft);
datafft = abs(datafft_);
imagesc(datafft)
feat2 = extract_feature_matlab(ch1, 48e3);

subplot(211)
plot(ch1)


%%
    
    c_point = 3244;
    N = 68;
    F=(0:nfft-1)*(fs/nfft);
    f=F(c_point-N:c_point+N-1);
=======
ch1 = textread("log.txt","%f");
s1 = textread("logfft.txt","%f");
% feat = extract_feature_matlab(ch, 48e3)

%%
    fs =48e3;
    nfft = 8192;
    step = 2048;
    window = rectwin(8192);
    c_point = 3244;
    N = 8;
    F=(0:nfft-1)*(fs/nfft);
    f=F(c_point-N+1:c_point+N);
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
    
    [tf_ch1,~,~,~]=spectrogram(ch1,window,nfft-step,nfft);
    Ep_front_ch1=zeros(1,length(f));
    
<<<<<<< HEAD
    
=======
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
    for idx_frame=1:size(tf_ch1,2)
        Spect_ch1=tf_ch1(:,idx_frame);
%         Spect_ch2=tf_ch2(:,idx_frame);
%         tf_gest_ch1(1:end-1,:)=tf_gest_ch1(2:end,:);%137*70*2
%         tf_gest_ch2(1:end-1,:)=tf_gest_ch2(2:end,:);
        [sp2_ch1,feature_ch1, Ep_front_ch1]=exact_feature(Spect_ch1,c_point,N,window,f, Ep_front_ch1);
%         [sp2_ch2,feature_ch2, Ep_front_ch2]=exact_feature(Spect_ch2,c_point,N,window,f, Ep_front_ch2);
        feature(:,idx_frame)=feature_ch1;
       
    end
    
<<<<<<< HEAD
    temp_fft_window = abs(tf_ch1(c_point-N:c_point+N-1, :))/sum(abs(window)*0.5);
    
=======
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
function [sp2,feature,Ep_front]=exact_feature(Spect,c_point,N,window,f,Ep_front)
    sp=Spect(c_point-N:c_point+N-1)';
    Ep=sp.*conj(sp)/(0.5*sum(window)^2);%ÄÜÁ¿

    Ep_m=abs(sp)/(0.5*sum(window));%·ù¶È

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
<<<<<<< HEAD
    sdecrease=sum((Ep_m(2:end)-Ep_m(1))./(1:length(Ep_m)-1))/sum(Ep_m(2:end));
=======
    sdecrease=sum((Ep(2:end)-Ep(1))./(1:length(Ep)-1))/sum(Ep(2:end));
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
    c = cumsum(Ep2);
    d = c(end)*0.7;
    idx = find(c>= d,1);
    srolloffPoint = f(idx);
    [s_max,s_loc]=max(sp2);
    feature=[rms;scentroid; sspread;sskewness;skurtosis;sentroy; sflat; screst;sflux;sslope; sdecrease;s_max;s_loc];
end

%% extract_feature2
function feature2=extract_feature_matlab(ch1,fs)
  SEntropy = spectralEntropy(ch1,fs, ...
<<<<<<< HEAD
        'Window', hamming(8192), ...
=======
        'Window', rectwin(8192), ...
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    SCentroid     = spectralCentroid(ch1,fs, ...
<<<<<<< HEAD
        'Window', hamming(8192), ...
=======
        'Window', rectwin(8192), ...
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);    
    
    SCrest        = spectralCrest(ch1,fs, ...
<<<<<<< HEAD
        'Window', hamming(8192), ...
=======
        'Window', rectwin(8192), ...
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SFlux         = spectralFlux(ch1,fs, ...
<<<<<<< HEAD
        'Window', hamming(8192), ...
=======
        'Window', rectwin(8192), ...
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SKurtosis     = spectralKurtosis(ch1,fs, ...
<<<<<<< HEAD
        'Window', hamming(8192), ...
=======
        'Window', rectwin(8192), ...
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SRolloffPoint = spectralRolloffPoint(ch1,fs, ...
<<<<<<< HEAD
        'Window', hamming(8192), ...
=======
        'Window', rectwin(8192), ...
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SSkewness     = spectralSkewness(ch1,fs, ...
<<<<<<< HEAD
        'Window', hamming(8192), ...
=======
        'Window', rectwin(8192), ...
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SSlope        = spectralSlope(ch1,fs, ...
<<<<<<< HEAD
        'Window', hamming(8192), ...
=======
        'Window', rectwin(8192), ...
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SSpread        = spectralSpread(ch1,fs, ...
<<<<<<< HEAD
        'Window', hamming(8192), ...
=======
        'Window', rectwin(8192), ...
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SDecrease        = spectralDecrease(ch1,fs, ...
<<<<<<< HEAD
        'Window', hamming(8192), ...
=======
        'Window', rectwin(8192), ...
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
    
    
    SFlatness      = spectralFlatness(ch1,fs, ...
<<<<<<< HEAD
        'Window', hamming(8192), ...
=======
        'Window', rectwin(8192), ...
>>>>>>> 0c39f00d3059dd1605821733e23d760b8870bdcd
        'OverlapLength',round(8192-2048), ...
        'Range',[18.6e3,19.4e3]);
     feature2=[SCentroid'; SSpread'; SSkewness'; SKurtosis'; SEntropy'; SFlatness';SCrest';SFlux';SSlope'; SDecrease';SRolloffPoint'];
end


