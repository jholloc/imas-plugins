function out = myFunction(request,varargin)

persistent cache

if ~isfield(cache,'S')
    cache.S = generate_S();
    source = 'regen';
else
    source = 'cache';
end

out = sprintf('%s (%s)',cache.S.(request),source);

end

function S = generate_S()
    S.S1 = 'TCV';
    S.S2 = 'AUG';
    S.S3 = 'JET';
end