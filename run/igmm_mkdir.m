function igmm_mkdir(dir)
    if (~exist(dir,'dir'))
        mkdir(dir)
        mkdir([ dir '/plots' ]);
    end
end