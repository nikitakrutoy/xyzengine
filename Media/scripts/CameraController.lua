Entity = {
    Properties = {
        CameraController = 1,
    },
    
    up_vector = Vector3(0.0, 1.0, 0.0),
    forward_vector = Vector3(0.0, 0.0, -1.0),
    right_vector = Vector3(1.0, 0.0, 0.0),
}

Entity.OnInit = function()
    Transform.Orientation = Quaternion(Radian(0.0), Entity.up_vector);
end

Entity.OnUpdate = function(dt)
    local speed = 100.0;
    if (inputHandler:isCommandActive("MoveForward")) then
        CameraTransform.Position = CameraTransform.Position + CameraTransform.Orientation * Entity.forward_vector * speed * dt;
    end
    if (inputHandler:isCommandActive("MoveBack")) then
        CameraTransform.Position = CameraTransform.Position + CameraTransform.Orientation * Entity.forward_vector * (-speed) * dt;
    end
    if (inputHandler:isCommandActive("MoveRight")) then
        CameraTransform.Position = CameraTransform.Position + CameraTransform.Orientation * Entity.right_vector * (speed) * dt;
    end
    if (inputHandler:isCommandActive("MoveLeft")) then
        CameraTransform.Position = CameraTransform.Position + CameraTransform.Orientation * Entity.right_vector * (-speed) * dt;
    end
    if (inputHandler:isCommandActive("MoveUp")) then
        CameraTransform.Position = CameraTransform.Position + CameraTransform.Orientation * Entity.up_vector * (speed) * dt;
    end
    if (inputHandler:isCommandActive("MoveDown")) then
        CameraTransform.Position = CameraTransform.Position + CameraTransform.Orientation * Entity.up_vector * (-speed) * dt;
    end
    if (inputHandler:GetMouse(3)) then
        local diffx = -inputHandler:GetMouseDiffX();
        local diffy = -inputHandler:GetMouseDiffY();
        local axis_up = CameraTransform.Orientation * Entity.up_vector;
        local axis_right = CameraTransform.Orientation * Entity.right_vector;
        local rot = Quaternion(Radian(diffy), axis_right) + Quaternion(Radian(diffx), axis_up);
        local rot1 = Quaternion(Radian(diffx), Entity.up_vector);
        local rot2 = Quaternion(Radian(diffy), Entity.right_vector);
        CameraTransform.Orientation = rot1 + CameraTransform.Orientation + rot2;
    end
end

