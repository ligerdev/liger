clc;clear;
%% create a distribution bounded between [3 100] with two peaks
width1 = 0.1; mean1 = 0;   %narrow peak
width2 = 0.6; mean2 = 0.5; %symetric and wider
ratios = [3,1];
N = 2500;
[cdf1,pdf1,z] = peak(width1,mean1,N);
[cdf2,pdf2]   = peak(width2,mean2,N);
pdfFunctions  = [pdf1 , pdf2];
cdfFunctions  = [cdf1 , cdf2];

augmentedPdf = augment(pdfFunctions,ratios);
augmentedCdf = augment(cdfFunctions,ratios);

% Different ratios
ratiosNew = [1,3];
augmentedPdfR = augment(pdfFunctions,ratiosNew);
augmentedCdfR = augment(cdfFunctions,ratiosNew);

% Shift and scale
[augmentedPdf,zNew] = shiftAndScale(augmentedPdf,3,100,z);
augmentedPdfR       = shiftAndScale(augmentedPdfR,3,100,z);

figure(1)
subplot(2,2,1)
plot(z,pdfFunctions);
title('Probability Functions');
legend(['peak(' num2str(width1) ',' num2str(mean1) ')'],...
    ['peak(' num2str(width2) ',' num2str(mean2) ')']);

subplot(2,2,2)
plot(z,cdfFunctions);
title('Cumulative Distribution Functions');
legend(['peak(' num2str(width1) ',' num2str(mean1) ')'],...
    ['peak(' num2str(width2) ',' num2str(mean2) ')'],...
    'location','southeast');

subplot(2,2,3)
plot(zNew,augmentedPdf,zNew,augmentedPdfR);
title('Augmented Shifted and Scaled Probability Function');
legend(['Ratios = [' num2str(ratios) ']'],...
    ['Ratios = [' num2str(ratiosNew) ']'])

subplot(2,2,4)
plot(zNew,augmentedCdf,zNew,augmentedCdfR);
title('Augmented Shifted and Scaled CDF')
legend(['Ratios = [' num2str(ratios) ']'],...
    ['Ratios = [' num2str(ratiosNew) ']'],...
    'location','southeast');

%% Combine a linear, uniform, and a peak
linearStart = 0.3; linearEnd = 1;
uniformStart = 0; uniformEnd = 0.5;
widthPeak = 0.3; meanPeak = 0.7;

ratios = [5,1,5];

[cdfLin,pdfLin,z] = linearProbability(linearStart,linearEnd,'increase',N);
[cdfUni,pdfUni]   = uniform(uniformStart,uniformEnd,N);
[cdfPeak,pdfPeak] = peak(widthPeak,meanPeak,N);
pdfFunctions  = [pdfLin , pdfUni , pdfPeak];
cdfFunctions  = [cdfLin , cdfUni , cdfPeak];

augmentedPdf = augment(pdfFunctions,ratios);
augmentedCdf = augment(cdfFunctions,ratios);

figure(2)
subplot(2,2,1)
plot(z,pdfFunctions);
title('Probability Functions');
legend(['Linear(' num2str(linearStart) ',' num2str(linearEnd) ')'],...
    ['Uniform(' num2str(uniformStart) ',' num2str(uniformEnd) ')'],...
    ['Peak(' num2str(widthPeak) ',' num2str(meanPeak) ')']);

subplot(2,2,2)
plot(z,cdfFunctions);
title('Cumulative Distribution Functions');
legend(['Linear(' num2str(linearStart) ',' num2str(linearEnd) ')'],...
    ['Uniform(' num2str(uniformStart) ',' num2str(uniformEnd) ')'],...
    ['Peak(' num2str(widthPeak) ',' num2str(meanPeak) ')'],...
    'location','southeast');

subplot(2,2,3)
plot(z,augmentedPdf);
title('Augmented Probability Function');
legend(['Ratios = [' num2str(ratios) ']'])

subplot(2,2,4)
plot(z,augmentedCdf);
title('Augmented CDF')
legend(['Ratios = [' num2str(ratios) ']'],...
    'location','southeast');
