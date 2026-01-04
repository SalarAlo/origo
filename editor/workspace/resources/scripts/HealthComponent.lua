Component.define("HealthComponent", {
	current_health = {
		type = "int",
		default = 10
	},
	max_health = {
		type = "int",
		default = 10
	},
	minimum_health = {
		type = "float",
		default = 0
	},
	is_dead = {
		type = "bool",
		default = false
	}
})

print("Hello from lua")
