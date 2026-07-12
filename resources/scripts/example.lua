local elapsed = 0.0

function start()
    sge.log("example.lua started for entity", entity.id)
end

function update(dt)
    elapsed = elapsed + dt
    entity:set_position(3.0, 1.0 + math.sin(elapsed) * 0.5, 0.0)
end
