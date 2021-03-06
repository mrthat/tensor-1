function tensor_write(fd,X,name)

% Extract the number of nonzeros and number of dimensions
nz = nnz(X);

if ~exist('name','var')
    name = 'ans';
end

if (nz == 0)
    fprintf('%s is an all-zero sparse tensor of size %s\n',...
        name, tt_size2str(X.size));
    return;
else
    fprintf(fd, '%s is a sparse tensor of size %s with %d nonzeros\n',...
        name, tt_size2str(X.size), nz);
end

% Stop insane printouts
%if (nz > 10000)
%    r = input('Are you sure you want to print all nonzeros? (Y/N) ','s');
%    if upper(r) ~= 'Y', return, end;
%end

% preallocate
output = cell(nz,1);
%%
spc = floor(log10(max(double(X.subs),[],1)))+1;
if numel(spc) == 1
    fmt = ['\t(%' num2str(spc(1)) 'd)%s'];
else
    fmt = ['\t(%' num2str(spc(1)) 'd,'];
    for i = 2:numel(spc)-1
        fmt = [fmt '%' num2str(spc(i)) 'd,'];
    end
    fmt = [fmt '%' num2str(spc(end)) 'd)%s'];
end
%%
% Get values out so that they look nice
savefmt = get(0,'FormatSpacing');
format compact
S = evalc('disp(X.vals)');
set(0,'FormatSpacing',savefmt)
S = textscan(S,'%s','delimiter','\n','whitespace','');
S = S{1};
if ~isempty(strfind(S{1},'*'))
    fprintf(fd,'%s\n',S{1});
    S = S(2:end);
end
%%
for i = 1:nz
    output{i} = sprintf(fmt,X.subs(i,:),S{i});
end
fprintf(fd,'%s\n',output{:});

%     function y = fmt(s,v)
%         % nested function has access to n from disp function workspace
%         if n > 1
%             y = [sprintf('\t(') sprintf('%d,',s(1:n-1))...
%                 sprintf('%d) ',s(n)) sprintf('\t%g',v)];
%         else
%             y = sprintf('\t(%d) \t%f',s,v);
%         end
%     end

end
