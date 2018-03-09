function res = findClick(verts, pt)
% Locate the index of the vertex in the vert list that's closest to pt. 
xys = verts(:, [1 2]);
k = size(verts, 1);
pts = repmat (pt, k, 1);
diffs = xys - pts;
sqdist = dot(diffs', diffs');
[c, res] = min(sqdist);

