loop = 0
self = 0
m = {}
m2 = {}
m3 = {}
angle = 0.0

MainLoop = function(delta)
	loop = loop + 1
	if loop > 50 then
		--engine.terminateScript(self)
	end
	angle = angle + 0.5
	m:setRotate(angle, 0, 1, 0)
	m2:setRotate(-angle, 0, 1, 0)
	m3:setRotate(angle*2, 0, 1, 0)
end

init = function(id)
	self = id
	engine.io.printMessage("Script is Ok")
	engine.setProjection(60, 0.3, 100)
	m = engine.Model("models/Borderlands 2 - Maya/maya.obj")
	m2 = engine.Model("models/Borderlands 2 - Maya/maya.obj")
	m3 = engine.Model("models/spider/spider.obj")

	m:setScale(5.0, 5.0, 5.0)
	m:setTranslate(-5.0, -5.0, -10.0)

	m2:setScale(5.0, 5.0, 5.0)
	m2:setTranslate(5.0, -5.0, -10.0)
	scale = 0.08
	m3:setScale(scale, scale, scale)
	m3:setTranslate(0.0, -2.0, -20.0)
end

deinit = function()
	engine.io.printMessage("deinit script")
end
