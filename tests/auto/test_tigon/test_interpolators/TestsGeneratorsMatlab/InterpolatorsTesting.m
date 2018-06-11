close all
fileID = fopen('interpolatortests.txt','w');
%% Generate tests for Linear Interpolator
%% Sine test
NP = 2;
N  = 4*NP;
NU = 10*N;
P  = 3*pi;
xs = 0;
xe = 2*P;
p1 = 2*pi/P;
fname = 'sin';
[x,y,xu,yu,xql,yql,xqs,yqs] = getestimates(xs,xe,N,NU,fname,p1);
figure(1)
plot(x,y,'ro',xql,yql,'b.',xqs,yqs,'gs',xu,yu,'r');

% LinearInterpolatorSine()
fprintf(fileID,'void tst_interpolators::test_LinearInterpolatorSine()\n');
fprintf(fileID,'{\n');
fprintf(fileID,'    using namespace Tigon;\n');
fprintf(fileID,'    QVector<qreal> xv;\n');
fprintf(fileID,'    QVector<qreal> yv;\n\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    cout << "Linear Interpolator - Sine function" << endl;\n');
fprintf(fileID,'    cout << "-----------------------------------" << endl;\n');
% Input
blockprint(x,'xv',fileID);
% Output
blockprint(y,'yv',fileID);

% Definitions
fprintf(fileID,'    LinearInterpolator* interpolator = new LinearInterpolator(xv,yv);\n');

fprintf(fileID,'    // Query vectors, expected output, actual output and errors\n');
fprintf(fileID,'    QVector<qreal> xq;\n');
fprintf(fileID,'    QVector<qreal> yq;\n');
fprintf(fileID,'    QVector<qreal> ya;\n');
fprintf(fileID,'    qreal maxerr = 0.0;\n\n');

% Linear query
blockprint(xu,'xq',fileID);

% Linear output
blockprint(yu,'yq',fileID);

fprintf(fileID,'    ya = interpolator->interpolateV(xq);\n');
fprintf(fileID,'    int sz = ya.size();\n');
fprintf(fileID,'    for(int i=0; i<sz; i++) {\n');
fprintf(fileID,'        qreal error = qAbs(ya[i]-yq[i]);\n');
fprintf(fileID,'        if(error>maxerr) {\n');
fprintf(fileID,'            maxerr = error;\n');
fprintf(fileID,'        }\n');
fprintf(fileID,'    }\n\n');
fprintf(fileID,'    cout << "Maximum error max(abs(ideal - interpolated)) = " << maxerr << endl;\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    TCOMPARE((maxerr<0.5), true);\n\n');
fprintf(fileID,'    delete interpolator;\n');
fprintf(fileID,'}\n');

%% Exponential test
NP = 2;
N  = 4*NP;
NU = 10*N;
P  = 3*pi;
xs = 0;
xe = 2*P;
p1 = 2*pi/P;
fname = 'exp';
[x,y,xu,yu,xql,yql,xqs,yqs] = getestimates(xs,xe,N,NU,fname,p1);
figure(2)
plot(x,y,'ro',xql,yql,'b.',xqs,yqs,'gs',xu,yu,'r');

% LinearInterpolatorExp()
fprintf(fileID,'void tst_interpolators::test_LinearInterpolatorExp()\n');
fprintf(fileID,'{\n');
fprintf(fileID,'    using namespace Tigon;\n');
fprintf(fileID,'    QVector<qreal> xv;\n');
fprintf(fileID,'    QVector<qreal> yv;\n\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    cout << "Linear Interpolator - Exponential function" << endl;\n');
fprintf(fileID,'    cout << "------------------------------------------" << endl;\n');
% Input
blockprint(x,'xv',fileID);
% Output
blockprint(y,'yv',fileID);

% Definitions
fprintf(fileID,'    LinearInterpolator* interpolator = new LinearInterpolator(xv,yv);\n');

fprintf(fileID,'    // Query vectors, expected output, actual output and errors\n');
fprintf(fileID,'    QVector<qreal> xq;\n');
fprintf(fileID,'    QVector<qreal> yq;\n');
fprintf(fileID,'    QVector<qreal> ya;\n');
fprintf(fileID,'    qreal maxerr = 0.0;\n\n');

% Linear query
blockprint(xu,'xq',fileID);

% Linear output
blockprint(yu,'yq',fileID);

fprintf(fileID,'    ya = interpolator->interpolateV(xq);\n');
fprintf(fileID,'    int sz = ya.size();\n');
fprintf(fileID,'    for(int i=0; i<sz; i++) {\n');
fprintf(fileID,'        qreal error = qAbs(ya[i]-yq[i]);\n');
fprintf(fileID,'        if(error>maxerr) {\n');
fprintf(fileID,'            maxerr = error;\n');
fprintf(fileID,'        }\n');
fprintf(fileID,'    }\n\n');
fprintf(fileID,'    cout << "Maximum error max(abs(ideal - interpolated)) = " << maxerr << endl;\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    TCOMPARE((maxerr<1e5), true);\n\n');
fprintf(fileID,'    delete interpolator;\n');
fprintf(fileID,'}\n');

%% Cumulative density function
% Combine a linear, uniform, and a peak - From Uncertainty Generator
% Examples
N = 500;
NU= 2500;
linearStart = 0.3; linearEnd = 1;
uniformStart = 0; uniformEnd = 0.5;
widthPeak = 0.3; meanPeak = 0.7;
ratios = [5,1,5];
[cdfLin,pdfLin,z] = linearProbability(linearStart,linearEnd,'increase',N);
[cdfUni,pdfUni]   = uniform(uniformStart,uniformEnd,N);
[cdfPeak,pdfPeak] = peak(widthPeak,meanPeak,N);
cdfFunctions  = [cdfLin , cdfUni , cdfPeak];
augmentedCdf = augment(cdfFunctions,ratios);

x = z;
y = augmentedCdf;

linearStart = 0.3; linearEnd = 1;
uniformStart = 0; uniformEnd = 0.5;
widthPeak = 0.3; meanPeak = 0.7;
ratios = [5,1,5];
[cdfLin,pdfLin,z] = linearProbability(linearStart,linearEnd,'increase',NU);
[cdfUni,pdfUni]   = uniform(uniformStart,uniformEnd,NU);
[cdfPeak,pdfPeak] = peak(widthPeak,meanPeak,NU);
cdfFunctions  = [cdfLin , cdfUni , cdfPeak];
augmentedCdf = augment(cdfFunctions,ratios);

xu= z;
yu= augmentedCdf;
xs = 0;
xe = 1;

% Linear Interpolation
xql = linspace(xs,xe,NU);
yql = interp1(x,y,xql,'linear');
% Cubic Spline Interpolation
xqs = xql;
yqs = interp1(x,y,xqs,'spline');

figure(3)
plot(x,y,'ro',xql,yql,'b.',xqs,yqs,'gs',xu,yu,'r');

% LinearInterpolatorExp()
fprintf(fileID,'void tst_interpolators::test_LinearInterpolatorCDF()\n');
fprintf(fileID,'{\n');
fprintf(fileID,'    using namespace Tigon;\n');
fprintf(fileID,'    QVector<qreal> xv;\n');
fprintf(fileID,'    QVector<qreal> yv;\n\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    cout << "Linear Interpolator - Cumulative density function" << endl;\n');
fprintf(fileID,'    cout << "-------------------------------------------------" << endl;\n');
% Input
blockprint(x,'xv',fileID);
% Output
blockprint(y,'yv',fileID);

% Definitions
fprintf(fileID,'    LinearInterpolator* interpolator = new LinearInterpolator(xv,yv);\n');

fprintf(fileID,'    // Query vectors, expected output, actual output and errors\n');
fprintf(fileID,'    QVector<qreal> xq;\n');
fprintf(fileID,'    QVector<qreal> yq;\n');
fprintf(fileID,'    QVector<qreal> ya;\n');
fprintf(fileID,'    qreal maxerr = 0.0;\n\n');

% Linear query
blockprint(xu,'xq',fileID);

% Linear output
blockprint(yu,'yq',fileID);

fprintf(fileID,'    ya = interpolator->interpolateV(xq);\n');
fprintf(fileID,'    int sz = ya.size();\n');
fprintf(fileID,'    for(int i=0; i<sz; i++) {\n');
fprintf(fileID,'        qreal error = qAbs(ya[i]-yq[i]);\n');
fprintf(fileID,'        if(error>maxerr) {\n');
fprintf(fileID,'            maxerr = error;\n');
fprintf(fileID,'        }\n');
fprintf(fileID,'    }\n\n');
fprintf(fileID,'    cout << "Maximum error max(abs(ideal - interpolated)) = " << maxerr << endl;\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    TCOMPARE((maxerr<0.001), true);\n\n');
fprintf(fileID,'    delete interpolator;\n');
fprintf(fileID,'}\n');

%% Probability density function
% Combine a linear, uniform, and a peak - From Uncertainty Generator
% Examples
N = 500;
NU= 2500;
linearStart = 0.3; linearEnd = 1;
uniformStart = 0; uniformEnd = 0.5;
widthPeak = 0.3; meanPeak = 0.7;
ratios = [5,1,5];
[cdfLin,pdfLin,z] = linearProbability(linearStart,linearEnd,'increase',N);
[cdfUni,pdfUni]   = uniform(uniformStart,uniformEnd,N);
[cdfPeak,pdfPeak] = peak(widthPeak,meanPeak,N);
pdfFunctions  = [pdfLin , pdfUni , pdfPeak];
augmentedPdf = augment(pdfFunctions,ratios);

x = z;
y = augmentedPdf;

linearStart = 0.3; linearEnd = 1;
uniformStart = 0; uniformEnd = 0.5;
widthPeak = 0.3; meanPeak = 0.7;
ratios = [5,1,5];
[cdfLin,pdfLin,z] = linearProbability(linearStart,linearEnd,'increase',NU);
[cdfUni,pdfUni]   = uniform(uniformStart,uniformEnd,NU);
[cdfPeak,pdfPeak] = peak(widthPeak,meanPeak,NU);
pdfFunctions  = [pdfLin , pdfUni , pdfPeak];
augmentedPdf = augment(pdfFunctions,ratios);

xu= z;
yu= augmentedPdf;
xs = 0;
xe = 1;

% Linear Interpolation
xql = linspace(xs,xe,NU);
yql = interp1(x,y,xql,'linear');
% Cubic Spline Interpolation
xqs = xql;
yqs = interp1(x,y,xqs,'spline');

figure(4)
plot(x,y,'ro',xql,yql,'b.',xqs,yqs,'gs',xu,yu,'r');

% LinearInterpolatorExp()
fprintf(fileID,'void tst_interpolators::test_LinearInterpolatorPDF()\n');
fprintf(fileID,'{\n');
fprintf(fileID,'    using namespace Tigon;\n');
fprintf(fileID,'    QVector<qreal> xv;\n');
fprintf(fileID,'    QVector<qreal> yv;\n\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    cout << "Linear Interpolator - Probability density function" << endl;\n');
fprintf(fileID,'    cout << "-------------------------------------------------" << endl;\n');
% Input
blockprint(x,'xv',fileID);
% Output
blockprint(y,'yv',fileID);

% Definitions
fprintf(fileID,'    LinearInterpolator* interpolator = new LinearInterpolator(xv,yv);\n');

fprintf(fileID,'    // Query vectors, expected output, actual output and errors\n');
fprintf(fileID,'    QVector<qreal> xq;\n');
fprintf(fileID,'    QVector<qreal> yq;\n');
fprintf(fileID,'    QVector<qreal> ya;\n');
fprintf(fileID,'    qreal maxerr = 0.0;\n\n');

% Linear query
blockprint(xu,'xq',fileID);

% Linear output
blockprint(yu,'yq',fileID);

fprintf(fileID,'    ya = interpolator->interpolateV(xq);\n');
fprintf(fileID,'    int sz = ya.size();\n');
fprintf(fileID,'    for(int i=0; i<sz; i++) {\n');
fprintf(fileID,'        qreal error = qAbs(ya[i]-yq[i]);\n');
fprintf(fileID,'        if(error>maxerr) {\n');
fprintf(fileID,'            maxerr = error;\n');
fprintf(fileID,'        }\n');
fprintf(fileID,'    }\n\n');
fprintf(fileID,'    cout << "Maximum error max(abs(ideal - interpolated)) = " << maxerr << endl;\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    TCOMPARE((maxerr<0.3), true);\n\n');
fprintf(fileID,'    delete interpolator;\n');
fprintf(fileID,'}\n');


%% Generate tests for Cubic Spline Interpolator
%% Sine test
NP = 2;
N  = 4*NP;
NU = 10*N;
P  = 3*pi;
xs = 0;
xe = 2*P;
p1 = 2*pi/P;
fname = 'sin';
[x,y,xu,yu,xql,yql,xqs,yqs] = getestimates(xs,xe,N,NU,fname,p1);
figure(5)
% plot(x,y,'ro',xql,yql,'b.',xqs,yqs,'gs',xu,yu,'r');
plot(x,y,'ro',xql,yql,'b.',xqs,yqs,'gs',xu,yu,'r',xqs,ro,'ks');

% SplineInterpolatorSine()
fprintf(fileID,'void tst_interpolators::test_SplineInterpolatorSine()\n');
fprintf(fileID,'{\n');
fprintf(fileID,'    using namespace Tigon;\n');
fprintf(fileID,'    QVector<qreal> xv;\n');
fprintf(fileID,'    QVector<qreal> yv;\n\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    cout << "Spline Interpolator - Sine function" << endl;\n');
fprintf(fileID,'    cout << "-----------------------------------" << endl;\n');
% Input
blockprint(x,'xv',fileID);
% Output
blockprint(y,'yv',fileID);

% Definitions
fprintf(fileID,'    SplineInterpolator* interpolator = new SplineInterpolator(xv,yv);\n');

fprintf(fileID,'    // Query vectors, expected output, actual output and errors\n');
fprintf(fileID,'    QVector<qreal> xq;\n');
fprintf(fileID,'    QVector<qreal> yq;\n');
fprintf(fileID,'    QVector<qreal> ya;\n');
fprintf(fileID,'    qreal maxerr = 0.0;\n\n');

% Spline query
blockprint(xu,'xq',fileID);

% Spline output
blockprint(yu,'yq',fileID);

fprintf(fileID,'    ya = interpolator->interpolateV(xq);\n');
fprintf(fileID,'    int sz = ya.size();\n');
fprintf(fileID,'    for(int i=0; i<sz; i++) {\n');
fprintf(fileID,'        qreal error = qAbs(ya[i]-yq[i]);\n');
fprintf(fileID,'        if(error>maxerr) {\n');
fprintf(fileID,'            maxerr = error;\n');
fprintf(fileID,'        }\n');
fprintf(fileID,'    }\n\n');
fprintf(fileID,'    cout << "Maximum error max(abs(ideal - interpolated)) = " << maxerr << endl;\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    TCOMPARE((maxerr<0.1), true);\n\n');
fprintf(fileID,'    delete interpolator;\n');
fprintf(fileID,'}\n');

%% Exponential test
NP = 2;
N  = 100;
NU = 1000;
P  = 3*pi;
xs = 0;
xe = 2*P;
p1 = 2*pi/P;
fname = 'exp';
[x,y,xu,yu,xql,yql,xqs,yqs] = getestimates(xs,xe,N,NU,fname,p1);
figure(6)
plot(x,y,'ro',xql,yql,'b.',xqs,yqs,'gs',xu,yu,'r');

% SplineInterpolatorExp()
fprintf(fileID,'void tst_interpolators::test_SplineInterpolatorExp()\n');
fprintf(fileID,'{\n');
fprintf(fileID,'    using namespace Tigon;\n');
fprintf(fileID,'    QVector<qreal> xv;\n');
fprintf(fileID,'    QVector<qreal> yv;\n\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    cout << "Spline Interpolator - Exponential function" << endl;\n');
fprintf(fileID,'    cout << "------------------------------------------" << endl;\n');
% Input
blockprint(x,'xv',fileID);
% Output
blockprint(y,'yv',fileID);

% Definitions
fprintf(fileID,'    SplineInterpolator* interpolator = new SplineInterpolator(xv,yv);\n');

fprintf(fileID,'    // Query vectors, expected output, actual output and errors\n');
fprintf(fileID,'    QVector<qreal> xq;\n');
fprintf(fileID,'    QVector<qreal> yq;\n');
fprintf(fileID,'    QVector<qreal> ya;\n');
fprintf(fileID,'    qreal maxerr = 0.0;\n\n');

% Spline query
blockprint(xu,'xq',fileID);

% Spline output
blockprint(yu,'yq',fileID);

fprintf(fileID,'    ya = interpolator->interpolateV(xq);\n');
fprintf(fileID,'    int sz = ya.size();\n');
fprintf(fileID,'    for(int i=0; i<sz; i++) {\n');
fprintf(fileID,'        qreal error = qAbs(ya[i]-yq[i]);\n');
fprintf(fileID,'        if(error>maxerr) {\n');
fprintf(fileID,'            maxerr = error;\n');
fprintf(fileID,'        }\n');
fprintf(fileID,'    }\n\n');
fprintf(fileID,'    cout << "Maximum error max(abs(ideal - interpolated)) = " << maxerr << endl;\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    TCOMPARE((maxerr<250), true);\n\n');
fprintf(fileID,'    delete interpolator;\n');
fprintf(fileID,'}\n');

%% Cumulative density function
% Combine a linear, uniform, and a peak - From Uncertainty Generator
% Examples
N = 500;
NU= 2500;
linearStart = 0.3; linearEnd = 1;
uniformStart = 0; uniformEnd = 0.5;
widthPeak = 0.3; meanPeak = 0.7;
ratios = [5,1,5];
[cdfLin,pdfLin,z] = linearProbability(linearStart,linearEnd,'increase',N);
[cdfUni,pdfUni]   = uniform(uniformStart,uniformEnd,N);
[cdfPeak,pdfPeak] = peak(widthPeak,meanPeak,N);
cdfFunctions  = [cdfLin , cdfUni , cdfPeak];
augmentedCdf = augment(cdfFunctions,ratios);

x = z;
y = augmentedCdf;

linearStart = 0.3; linearEnd = 1;
uniformStart = 0; uniformEnd = 0.5;
widthPeak = 0.3; meanPeak = 0.7;
ratios = [5,1,5];
[cdfLin,pdfLin,z] = linearProbability(linearStart,linearEnd,'increase',NU);
[cdfUni,pdfUni]   = uniform(uniformStart,uniformEnd,NU);
[cdfPeak,pdfPeak] = peak(widthPeak,meanPeak,NU);
cdfFunctions  = [cdfLin , cdfUni , cdfPeak];
augmentedCdf = augment(cdfFunctions,ratios);

xu= z;
yu= augmentedCdf;
xs = 0;
xe = 1;

% Linear Interpolation
xql = linspace(xs,xe,NU);
yql = interp1(x,y,xql,'linear');
% Cubic Spline Interpolation
xqs = xql;
yqs = interp1(x,y,xqs,'spline');

figure(7)
plot(x,y,'ro',xql,yql,'b.',xqs,yqs,'gs',xu,yu,'r');

% SplineInterpolatorExp()
fprintf(fileID,'void tst_interpolators::test_SplineInterpolatorCDF()\n');
fprintf(fileID,'{\n');
fprintf(fileID,'    using namespace Tigon;\n');
fprintf(fileID,'    QVector<qreal> xv;\n');
fprintf(fileID,'    QVector<qreal> yv;\n\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    cout << "Spline Interpolator - Cumulative density function" << endl;\n');
fprintf(fileID,'    cout << "-------------------------------------------------" << endl;\n');
% Input
blockprint(x,'xv',fileID);
% Output
blockprint(y,'yv',fileID);

% Definitions
fprintf(fileID,'    SplineInterpolator* interpolator = new SplineInterpolator(xv,yv);\n');

fprintf(fileID,'    // Query vectors, expected output, actual output and errors\n');
fprintf(fileID,'    QVector<qreal> xq;\n');
fprintf(fileID,'    QVector<qreal> yq;\n');
fprintf(fileID,'    QVector<qreal> ya;\n');
fprintf(fileID,'    qreal maxerr = 0.0;\n\n');

% Spline query
blockprint(xu,'xq',fileID);

% Spline output
blockprint(yu,'yq',fileID);

fprintf(fileID,'    ya = interpolator->interpolateV(xq);\n');
fprintf(fileID,'    int sz = ya.size();\n');
fprintf(fileID,'    for(int i=0; i<sz; i++) {\n');
fprintf(fileID,'        qreal error = qAbs(ya[i]-yq[i]);\n');
fprintf(fileID,'        if(error>maxerr) {\n');
fprintf(fileID,'            maxerr = error;\n');
fprintf(fileID,'        }\n');
fprintf(fileID,'    }\n\n');
fprintf(fileID,'    cout << "Maximum error max(abs(ideal - interpolated)) = " << maxerr << endl;\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    TCOMPARE((maxerr<0.001), true);\n\n');
fprintf(fileID,'    delete interpolator;\n');
fprintf(fileID,'}\n');

%% Probability density function
% Combine a linear, uniform, and a peak - From Uncertainty Generator
% Examples
N = 500;
NU= 2500;
linearStart = 0.3; linearEnd = 1;
uniformStart = 0; uniformEnd = 0.5;
widthPeak = 0.3; meanPeak = 0.7;
ratios = [5,1,5];
[cdfLin,pdfLin,z] = linearProbability(linearStart,linearEnd,'increase',N);
[cdfUni,pdfUni]   = uniform(uniformStart,uniformEnd,N);
[cdfPeak,pdfPeak] = peak(widthPeak,meanPeak,N);
pdfFunctions  = [pdfLin , pdfUni , pdfPeak];
augmentedPdf = augment(pdfFunctions,ratios);

x = z;
y = augmentedPdf;

linearStart = 0.3; linearEnd = 1;
uniformStart = 0; uniformEnd = 0.5;
widthPeak = 0.3; meanPeak = 0.7;
ratios = [5,1,5];
[cdfLin,pdfLin,z] = linearProbability(linearStart,linearEnd,'increase',NU);
[cdfUni,pdfUni]   = uniform(uniformStart,uniformEnd,NU);
[cdfPeak,pdfPeak] = peak(widthPeak,meanPeak,NU);
pdfFunctions  = [pdfLin , pdfUni , pdfPeak];
augmentedPdf = augment(pdfFunctions,ratios);

xu= z;
yu= augmentedPdf;
xs = 0;
xe = 1;

% Linear Interpolation
xql = linspace(xs,xe,NU);
yql = interp1(x,y,xql,'linear');
% Cubic Spline Interpolation
xqs = xql;
yqs = interp1(x,y,xqs,'spline');

figure(8)
plot(x,y,'ro',xql,yql,'b.',xqs,yqs,'gs',xu,yu,'r');

% SplineInterpolatorExp()
fprintf(fileID,'void tst_interpolators::test_SplineInterpolatorPDF()\n');
fprintf(fileID,'{\n');
fprintf(fileID,'    using namespace Tigon;\n');
fprintf(fileID,'    QVector<qreal> xv;\n');
fprintf(fileID,'    QVector<qreal> yv;\n\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    cout << "Spline Interpolator - Probability density function" << endl;\n');
fprintf(fileID,'    cout << "-------------------------------------------------" << endl;\n');
% Input
blockprint(x,'xv',fileID);
% Output
blockprint(y,'yv',fileID);

% Definitions
fprintf(fileID,'    SplineInterpolator* interpolator = new SplineInterpolator(xv,yv);\n');

fprintf(fileID,'    // Query vectors, expected output, actual output and errors\n');
fprintf(fileID,'    QVector<qreal> xq;\n');
fprintf(fileID,'    QVector<qreal> yq;\n');
fprintf(fileID,'    QVector<qreal> ya;\n');
fprintf(fileID,'    qreal maxerr = 0.0;\n\n');

% Spline query
blockprint(xu,'xq',fileID);

% Spline output
blockprint(yu,'yq',fileID);

fprintf(fileID,'    ya = interpolator->interpolateV(xq);\n');
fprintf(fileID,'    int sz = ya.size();\n');
fprintf(fileID,'    for(int i=0; i<sz; i++) {\n');
fprintf(fileID,'        qreal error = qAbs(ya[i]-yq[i]);\n');
fprintf(fileID,'        if(error>maxerr) {\n');
fprintf(fileID,'            maxerr = error;\n');
fprintf(fileID,'        }\n');
fprintf(fileID,'    }\n\n');
fprintf(fileID,'    cout << "Maximum error max(abs(ideal - interpolated)) = " << maxerr << endl;\n');
fprintf(fileID,'    cout << endl;\n');
fprintf(fileID,'    TCOMPARE((maxerr<0.3), true);\n\n');
fprintf(fileID,'    delete interpolator;\n');
fprintf(fileID,'}\n');

%% Finalize
fclose(fileID);
