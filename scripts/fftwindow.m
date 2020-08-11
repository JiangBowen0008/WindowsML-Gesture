function [env, null_rms, spect, max_out] = fftwindow(data, fc)
nfft=8192;
step=2048;
c_point=ceil((fc/48e3) *8192) + 1;
dfc = 400;
dfc2 = 50;
n = floor((dfc/48e3) *8192);
dn2 = floor((dfc2/48e3) *8192);
window=hanning(nfft);

% data = bandpass(data,[fc-dfc fc+dfc],fs);

% data_fft= spectrogram(data, window, nfft - step, nfft, 'centered');
data_fft = spectrogram(data, window, nfft - step, nfft);
spect = abs(data_fft(c_point - n : c_point + n, :));
null_spect = abs(data_fft(c_point + 1.5 * n : c_point + 2.5 *n, :));
null_rms = mean(null_spect.^2, 1);
% null_var = var(null_spect.^2, 0, 1);
% null_val = null_rms ./ null_var; 
% spect = abs(spect(:, 2:end) - spect(:, 1:end-1));

% middle = spect(n - dn1: n + dn1, :);
% middle2 = spect(n - dn2: n + dn2, :);
len = size(spect, 1);
n = n + 1;
idx = [(1:n - dn2),(n + dn2 : len)];
out = spect(idx,:);

% mean_out = mean(out, 1);
% mean_out = movmean(mean_out, 30);
max_out = max(out, [], "all");
% spect = min(spect, max_out);

% norm_spect = spect - mean_out;
% norm_spect = min(norm_spect, max_out);

rms_out = sum(out.^2, 1);
env = envelope(rms_out, 10, 'peak');
% spect = out;
% rms = mean(spect.^2, 1) ;
% spect = log(spect);

end