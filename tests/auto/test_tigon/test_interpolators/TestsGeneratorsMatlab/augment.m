function p = augment(pFunctions,ratios)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Returns an augmented probability function from the %
% matrix psiNs according to the ratios vector        %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if nargin < 2
    ratios = ones(1,size(pFunctions,2));
end
% normalise the ratios:
ratios = ratios / sum(ratios);

p = sum(pFunctions.* repmat(ratios,size(pFunctions,1),1),2);