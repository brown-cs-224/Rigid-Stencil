function testscript2()
% Draws an n x n array of triangles, sort of close to the
% integer lattice. 
%
% User clicks on 2 of them -- these are "constrained" to stay in place
%
% The user then clicks and drags to a vertex v1 and a continually-varying target for it.


clf;
close;
n = 6; 
[x,y] = meshgrid(1:n,1:n);
x = x + 0.4 * rand(n);
y = y + 0.4 * rand(n);
z = zeros(n);
tri = delaunay(x,y);
mesh.vertices = [x(:), y(:), z(:)];
mesh.faces = tri;
k = size(mesh.faces, 1);
mesh.facevertexcdata = 0.4 + 0.6* rand(k, 3);
clf;

drawMesh(mesh);
set(gca, 'XLim', [0, n+2], 'YLim', [0, n+2]);

i2 = getClick(mesh.vertices);
i3 = getClick(mesh.vertices);
mdata = {mesh, i2, i3, n};
handles = guidata(gca);
handles.mdata = mdata;
guidata(gca,handles)

handleClickAndDrag()

function handleClickAndDrag()

%---handle initialization
handles = guidata(gca);
handles.macro_active=0;
guidata(gca,handles);

%---define callback routine on setup
handles.init_state = uisuspend(gcf);
guidata(gca,handles); %--save initial state
    
set(gcf, 'windowbuttondownfcn', {@myclick,1});
set(gcf, 'windowbuttonmotionfcn', {@myclick,2});
set(gcf, 'windowbuttonupfcn', {@myclick,3});
set(gcf, 'keypressfcn', {@myclick,4});

%--------function to handle event
function myclick(h,event,type)
handles=guidata(gca);

switch type
    case 1 %---Button down
        'Button down'
        
        mdata = handles.mdata;
        m.n = mdata{4};
        m.mesh = mdata{1};
        pt=get(gca,'CurrentPoint');
        'start'
        pt = pt(1, [1, 2])
        hold on
        plot(pt(1), pt(2), 'rs', 'MarkerEdgeColor', 'black', 'MarkerFaceColor', 'green', 'MarkerSize', 12);
        hold off;
        pause(0.4);
%        q1 = newPoint();
        i1 = findClick(m.mesh.vertices, pt);
        m.constrained = [i1, mdata{2}, mdata{3}];
        m.start = pt;
        m.target2 = m.mesh.vertices(m.constrained(2), :);
        m.target3 = m.mesh.vertices(m.constrained(3), :);
        handles.mdata = m;
        
%        set(gca,'NextPlot','replace')
%        set(gcf,'Pointer','fullcrosshair');
        handles.macro_active=1;
        xl=get(gca,'XLim');yl=get(gca,'YLim');
%        if ((handles.xpos0 > xl(1) & handles.xpos0 < xl(2)) & (handles.ypos0 > yl(1) & handles.ypos0 < yl(2))) %--disable if outside axes
%        end
        guidata(gca,handles)
    case 2%---Button Move
        if handles.macro_active
            'Drag'
            pt=get(gca,'CurrentPoint');
            'Drag point'
            pt = pt(1, [1, 2])
            m = handles.mdata;
            i1 = m.constrained(1);
            target = m.mesh.vertices(i1, :) + [(pt - m.start), 0];
            %set(gcf,'Pointer','fullcrosshair');
            cla
            mesh2 = deformMesh(m.mesh, m.constrained, [target; m.target2; m.target3]);
            drawMesh(mesh2);
            set(gca, 'XLim', [0, m.n+2], 'YLim', [0, m.n+2]);
            guidata(gca,handles)
        end
        
    case 3 %----Button up (cleanup some variable)
        'Button Release'
        if handles.macro_active
            
            set(gcf,'Pointer','arrow');
            set(gca,'NextPlot','add')
            handles.macro_active=0;
            uirestore(handles.init_state);
            guidata(gca,handles)
        end
        
    case 4 %----Key press
        guidata(gca,handles)
    end
    
    %---------function to pickup to pointed curve
    function [col,lstyle]=line_pickup(list_line_obj,pos)
    
    col=0;
    lstyle='-';
    
    %-define searching windows
    xl=get(gca,'XLim');
    xwin=abs(xl(1)-xl(2))/100;
    yl=get(gca,'YLim');
    ywin=abs(yl(1)-yl(2))/100;
    
    %-load all datasets
    for i=1:length(list_line_obj)
        xData{i}=get(list_line_obj(i), 'XData');
        yData{i}=get(list_line_obj(i), 'YData');
    end
    
%     %--look for matches in x and y direction
    for i=1:length(list_line_obj)
        candidate{i}=find((abs(pos(1,2)-yData{i})<ywin) & (abs(pos(1,1)-xData{i})<xwin));
    end
           
    %---find the right guy
 for i=1:length(list_line_obj)
     if ~isempty(candidate{i})
         col=i;
         lstyle=get(list_line_obj(col),'LineStyle');
         if lstyle==':'
         set(list_line_obj(col),'LineStyle','-');
         else
         set(list_line_obj(col),'LineStyle',':');
         end
     end
 end