function answ = myFunction2(request,shot,indices,dtype)

  persistent cache

  try
    [ids_type,S,shape_of] = preprocess_request(request,indices);

    ids_name = sprintf('%s_%d',ids_type,shot);

    if ~isfield(cache,ids_name)
      cache.(ids_name) = generate_ids(ids_type,shot);
      source = 'regen';
    else
      source = 'cache';
    end
    
    if shape_of
      out = numel(subsref(cache.(ids_name),S));
    else
      if isempty(S)
        out = cache.(ids_name);
      else
        out = subsref(cache.(ids_name),S);
      end 
    end
    
    out=cast_type(out,dtype);
    
    status = 0;
    
    message = source;
  catch ME
    out = [];
    status = 1;
    message = ME.getReport();
  end
  
  answ = {out,status,message};
  
end

function [ids_type,S,shape_of] = preprocess_request(request,indices)
  delim = '/';
  index_tok = '#';
  tokens = split(request,delim);
  % Avoid any leading slashes
  if ~isempty(tokens) && isempty(tokens{1}),
    tokens = tokens(2:end);
  end
  % Extract ids_type
  ids_type = tokens{1};
  tokens = tokens(2:end);
  
  nlevel = numel(tokens);
  
  nindex = sum(strcmp(index_tok,tokens));
  if nindex ~= numel(indices),
    error('Badly formed request, number of indices must match count of #');
  end
  
  S = cell(2,1+nlevel);
  S(:,1) = {'()',{':'}};
  iindex = 1;
  for ii = 1:nlevel,
    if strcmp(tokens{ii},index_tok)
      S(:,ii+1) = {'{}';{indices(iindex)}};
      iindex = iindex+1;
    else
      S(:,ii+1) = {'.';tokens{ii}};
    end
  end
  S = substruct(S{:});
  
  shape_of = ~isempty(S) && strcmp(S(end).type,'.') && strcmpi(S(end).subs,'shape_of');
  if shape_of
    S = S(1:end-1);
  end
end

function ids = generate_ids(ids_type,shot)
  
  persistent init
  
  if isempty(init) || ~init,
    
    if isempty(which('mdsipmex'))
      addpath('~g2osaute/public/matlab9_11_2016');
    end
    if isempty(which('ids_gen'))
      addpath(fullfile(getenv('IMAS_PREFIX'),'matlab'));
      javaaddpath(fullfile(getenv('IMAS_PREFIX'),'jar/imas.jar'));
    end
    if isempty(which('gdat'))
      addpath('~g2osaute/public/matlab/gdat/crpptbx');
    end
    if isempty(which('tcv2ids'))
      addpath('~g2osaute/public/matlab/gdat/crpptbx/TCV_IMAS');        
    end
    if isempty(which('interpos'))
      addpath('~g2osaute/public/matlab');
    end
    
    init = true;
  end
  
  mdsconnect('localhost:8002');
  ids = getfield(gdat(shot,'ids','source',ids_type,'machine','TCV'),ids_type);
  if isempty(ids)
    error('gdat returned an empty structure for ids %s',ids_type);
  end
end

function out = cast_type(out,dtype)
  
  message = 'Incompatible return type for dtype=%d, out is %s but should be %s';
  
  persistent uda_types
  if isempty(uda_types)
    uda_types.uda_1  = 'int8';   % UDA_TYPE_CHAR 
    uda_types.uda_2  = 'int16';  % UDA_TYPE_SHORT 
    uda_types.uda_3  = 'int32';  % UDA_TYPE_INT
    uda_types.uda_4  = 'uint32'; % UDA_TYPE_UNSIGNED_INT 
    uda_types.uda_5  = 'int32';  % UDA_TYPE_LONG
    uda_types.uda_6  = 'single'; % UDA_TYPE_FLOAT
    uda_types.uda_7  = 'double'; % UDA_TYPE_DOUBLE
    uda_types.uda_8  = 'uint8';  % UDA_TYPE_UNSIGNED_CHAR
    uda_types.uda_9  = 'uint16'; % UDA_TYPE_UNSIGNED_SHORT
    uda_types.uda_10 = 'uint32'; % UDA_TYPE_UNSIGNED_LONG
    uda_types.uda_11 = 'int64';  % UDA_TYPE_LONG64
    uda_types.uda_12 = 'uint64'; % UDA_TYPE_UNSIGNED_LONG64
    uda_types.uda_17 = 'char';   % UDA_TYPE_STRING 
  end
  
  switch dtype,
    case {1,2,3,4,5,6,7,8,9,10,11,12}
      % assert(isa(out,'double'),message,dtype,class(out),'double');
      out = cast(out,uda_types.(sprintf('uda_%d',dtype)));
    case {17} % UDA_TYPE_STRING
              % assert(ischar(out) || isstring(out),message,dtype,class(out),'char or string');
      if iscell(out), out = strvcat(out{:});end
      out = cast(out,uda_types.(sprintf('uda_%d',dtype)));
    case {-1} % Allows all return types
    otherwise
      error('dtype %d not supported',dtype);
  end
end