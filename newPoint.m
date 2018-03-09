function res = newPoint()
% get a click and find the index of the closest vertex; draw a marker there
pt = ginput(1);
hold on;
plot(pt(1), pt(2), 'rs', 'MarkerEdgeColor', 'black', 'MarkerFaceColor', 'green', 'MarkerSize', 12);
hold off;

res = [pt];