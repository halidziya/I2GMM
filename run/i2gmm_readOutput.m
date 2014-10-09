function [ dishes rests likelihood labels]=i2gmm_readOutput(results_dir)
if (~exist('results_dir','var')) %Default value
    results_dir = ''; 
end
file=fopen([results_dir 'Dish.dish']);
restfile = fopen([results_dir 'Restaurant.rest']);
likefile = fopen([results_dir 'likelihood.matrix']);
labelsfile = fopen([results_dir 'Labels.matrix']);
dishes = [];
rests = [];
ndish=fread(file,1,'int');
nrest=fread(restfile,1,'int');

%fprintf(1,'Dishes %d',ndish);
for i=1:ndish
    dish=readDish(file);
    dishes = [dishes dish];
end
for i=1:nrest
    rest = readRest(restfile);
    rests = [rests rest];
end
ldim = fread(likefile,2,'int');
likelihood = fread(likefile,ldim(1),'double');
labels = readMat(labelsfile);
%ldim = fread(labelsfile,2,'int');
%labels = fread(labelsfile,ldim(1)*ldim(2),'double');
%labels = reshape(labels,ldim(2),ldim(1));
fclose(file);
fclose(restfile);
fclose(likefile);
fclose(labelsfile);
end

function dish=readDish(file)
    dish.id=fread(file,1,'int');
    dish.ntables = fread(file,1,'int');
    dish.nsamples = fread(file,1,'int');
    dish.kap=fread(file,1,'double');
    dish.kapi=fread(file,1,'double');
    dish.logprob=fread(file,1,'double');
    dish.scatter=readMat(file);
    dish.mean=readMat(file);
    dish.d = length(dish.mean);
    dish.dist = readStut(file);
end

function mat=readMat(file)
    r = fread(file,1,'int');
    d = fread(file,1,'int');
    mat=fread(file,r*d,'double');
    mat = reshape(mat,d,r); % Row major , column major difference
    % triangle = fread(file,1,'int');
end

function stut=readStut(file)
    stut.eta = fread(file,1,'double');
    stut.normalizer = fread(file,1,'double');
    stut.coef1 = fread(file,1,'double');
    stut.mu = readMat(file);
    stut.cholSigma = readMat(file);
end


function rest=readRest(file)
    rest.id = fread(file,1,'int');
    rest.likelihood = fread(file,1,'double');
    rest.ntables   =  fread(file,1,'int');
    rest.tables = readTables(file,rest.ntables);
    rest.ncustomers = fread(file,1,'int');
    rest.customers = readCust(file,rest.ncustomers);
    for i=1:length(rest.customers)
        rest.customers(i).dishid = rest.tables(rest.customers(i).tableid).dishid;
    end
end

function custs=readCust(file,n)
    cust.id = fread(file,1,'int');
    cust.likelihood0 = fread(file,1,'double');
    cust.tableid = fread(file,1,'int');
    cust.data = readMat(file);
    
    custs =  repmat(cust,1,n); % Just to speed up
    
    for i=2:n
        cust.id = fread(file,1,'int');
        cust.likelihood0 = fread(file,1,'double');
        cust.tableid = fread(file,1,'int');
        cust.data = readMat(file);
        custs(i) = cust;
    end
end

function tables=readTables(file,n)
  
    table.tableid = fread(file,1,'int');
    table.dishid = fread(file,1,'int');
    table.npoints = fread(file,1,'int');
    table.likelihood = fread(file,1,'double');
    table.scatter = readMat(file);
    table.mean = readMat(file);
    table.dist = readStut(file);
    
    
    tables = repmat(table,1,n); % Speed up
    
    for i=2:n
        table.tableid = fread(file,1,'int');
        table.dishid = fread(file,1,'int');
        table.npoints = fread(file,1,'int');
        table.likelihood = fread(file,1,'double');
        table.scatter = readMat(file);
        table.mean = readMat(file);
        table.dist = readStut(file);
        tables(i) = table;
    end
    
end

%function mat=readVec(file)
%    r = fread(file,1,'int');
%    d = fread(file,1,'int');
%    mat=fread(file,r*d,'double');
% end