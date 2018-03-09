function testscript()
% Draws an n x n array of triangles, sort of close to the
% integer lattice. 
%
% User clicks on 3 of them -- these are "constrained"
% Numbers 2 and 3 remain in place. 
% The user clicks once more to specify a target for v1 (shown as a green
% square, I think). 
%
% The program then draws the original mesh on the left and the new mesh on
% the right.

clf;
n = 6; 
[x,y] = meshgrid(1:n,1:n)
x = x + 0.4 * rand(n);
y = y + 0.4 * rand(n);
z = zeros(n);
tri = delaunay(x,y);
%mesh.vertices = [0 0 0; 1 0 0; 1 1 0; 0 1 0];
%mesh.faces  = [1 2 3; 1 3 4];
mesh.vertices = [x(:), y(:), z(:)];
mesh.faces = tri;
k = size(mesh.faces, 1);
mesh.facevertexcdata = rand(k, 3);
clf;

%mesh.vertices = [0 0 0; 1 0 0; 0 0.5 0];
%mesh.faces = [1 2 3];
%mesh.facevertexcdata = [0.6 0.4 0.3];

drawMesh(mesh);

i1 = getClick(mesh.vertices)
i2 = getClick(mesh.vertices);
i3 = getClick(mesh.vertices);
q1 = newPoint(); 

mesh2 = deformMesh(mesh, [i1, i2, i3], [[q1, 0]; mesh.vertices(i2, :); mesh.vertices(i3, :)]);

%mesh2 = deformMesh(mesh, [1, 2], [mesh.vertices(1,:); mesh.vertices(2, :) + [0.5, 0, 0]]);
figure(1);
h1 = subplot(1, 2, 1);
axis square
a1 = gca;
drawMesh(mesh);
constrained = mesh.vertices([i1, i2, i3], [1 2]);

hold on;
plot(constrained(:, 1), constrained(:, 2), 'rd', 'MarkerEdgeColor', 'black', 'MarkerFaceColor', 'red', 'MarkerSize', 12);
plot(q1(1), q1(2), 'rs', 'MarkerEdgeColor', 'black', 'MarkerFaceColor', 'green', 'MarkerSize', 12);
hold off;

h2 = subplot(1, 2, 2)
a2 = gca;
axis square
set(a2, 'XLim', get(a1, 'XLim'));
set(a2, 'YLim', get(a1, 'YLim'));

drawMesh(mesh2);
hold on;
plot(constrained(:, 1), constrained(:, 2), 'rd', 'MarkerEdgeColor', 'black', 'MarkerFaceColor', 'red', 'MarkerSize', 12);
plot(q1(1), q1(2), 'rs', 'MarkerEdgeColor', 'black', 'MarkerFaceColor', 'green', 'MarkerSize', 12);
hold off;