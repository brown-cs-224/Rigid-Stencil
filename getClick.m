function res = getClick(verts)
% get a click and find the index of the closest vertex; draw a marker there
pt = ginput(1);
res = findClick(verts, pt);

p1 = verts(res, [1 2]);
hold on;
plot(p1(1), p1(2), 'rd', 'MarkerEdgeColor', 'black', 'MarkerFaceColor', 'red', 'MarkerSize', 12);
hold off;