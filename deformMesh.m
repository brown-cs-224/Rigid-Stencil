% TODO: State what mesh, constrained, and targets are

function mesh2 = deformMesh(mesh, constrained, targets)
% i1, i2, i3, target1, target2, target3)
mesh2.vertices = mesh.vertices(:, [1 2]); % get rid of z-coord.
mesh2.faces = mesh.faces;
mesh2.facevertexcdata = mesh.facevertexcdata;

n = size(mesh2.vertices, 1); % number of vertices.
f = size(mesh2.faces, 1);   % number of faces.



% TODO: Your code goes here. 
% Follow what we did in class / what's in the original paper.



% For step 1, there's one error term for each triangle vertex in terms
%   of the other two vertices in the triangle. You'll likely find
%   the following data structure helpful: pfaces is a 3f x 3 matrix
%   where f is the number of faces in the mesh. Each row is a 1 x 3
%   integer vector giving the vertex indices of a face in some order.
%   There are three orders (and thus three rows) for each face: one
%   order with each of the three vertices occuring first.
% If we have a toy mesh with the following two faces:
%   1 2 3
%   2 3 4
% then pfaces will be a matrix that looks like:
%   1 2 3
%   2 3 4
%   3 1 2
%   4 2 3
%   2 3 1
%   3 4 2
faces1 = mesh.faces;
faces2 = mesh.faces(:, [3, 1, 2]);
faces3 = mesh.faces(:, [2, 3, 1]);
pfaces = [faces1; faces2; faces3];

% We also provide a utility function, findXYs, which uses this matrix
%   to compute the XY coordinates of each vertex relative to the other
%   two vertices in the triangle.
% Make sure you understand how this is equivalent to the math
%   we went over in class.
function xys = findXYs(verts, pfaces)
	v3v1 = e31 = verts(pfaces(:, 3), :) - verts(pfaces(:, 1), :);
	w = verts(pfaces(:, 2), :) - verts(pfaces(:, 1), :);
	w_perp = [-e21(:, 2), e21(:, 1)];
	w_dot_w = dot(e21, e21, 2);
	x = dot(v3v1, w, 2) ./ w_dot_w;
	% dot(w, w) == dot(w_perp, w_perp)
	y = dot(v3v1, w_perp, 2) ./ w_dot_w;
	xys = [x, y];
end


