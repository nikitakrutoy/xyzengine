Entity = {
    Properties = {
        CameraController = 0,
    },
    
    up_vector = Vector3(0.0, 1.0, 0.0),
    start = Vector3(0.0, 0.0, 0.0),
    time = 0;
}

Entity.OnInit = function()
    Transform.Orientation = Quaternion(Radian(0.0), Entity.up_vector);
end

Entity.OnUpdate = function(dt)
end
