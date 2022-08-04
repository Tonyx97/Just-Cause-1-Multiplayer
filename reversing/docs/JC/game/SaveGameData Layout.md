```cpp
string -> 1 byte (checksum?) -> 4 bytes (size) -> zero terminated string (total: 5 bytes + string size)

// general deserializer

	// local player deserializer

	hp scale (float)
	local matrix (mat4)
	ammo grenades (int)

		// weapon deserializer
	
		for i = 0, 14 do
			  slot i ammo (int)
		
		unknown WeaponBelt var (int)
		weapons count (int)
		
		for i = 0, weapons count do
			weapon i name (string)
			weapon i mag ammo (int)
		
		weapon to draw (int)
		unknown (int)
		unknown (int)
		unknown (int)

	character flag 0x888 (byte)
	
	if unknown (byte)
		for i = 0, 3 do
			unknown (float)

	if unknown (byte)
		unknown (mat4)
		unknown (int)

	unknown (float)
	unknown (int)
	unknown (float)
	unknown (float)

// general deserializer

	// spawn manager deserializer

	unknown (int)
	unknown (int)

	for i = 0, unknown count (int) do
		unknown (int)

// CLocation huge shit (TODO)

	// AiCore deserializer
	
		unknown (string)

	// unknown deserializer
	
		for i = 0, 4 do
			unknown (int)
			unknown (byte)

	// unknown deserializer

		for i = 0, unknown count (int) do
			unknown (int)
			
			for i = 0, 5 do
				unknown (int)
				
			unknown (int)
			unknown (int)
			
		for i = 0, unknown count (int) do
			unknown (int)
			unknown (int)
			unknown (int)
			unknown (mat4)

	// day cycle deserializer

	time (float)

	// unknown deserializer

	unknown (float)

	// dropoff deserializer

	unknown (int)

	// unknown deserializer

	unknown (byte)

	// stats system deserializer

	for i = 0, stat count (int) do
		name (string)
		unknown (string)
		unknown (int)
		unknown (int)
		unknown (float)

	for i = 0, stat 2 count (int) do
		name (string)
		unknown (int)
		unknown (int)

	total shots fired (int)
	story missions completed (int)
	side missions completed (int)

	for i = 0, 15 do
		unknown (int)

	for i = 0, 34 do
		unknown (bool)
	
	agency safehouses count (int)
	guerrilla safehouses count (int)
	rioja safehouses count (int)
	highest base jump (float)
	roof to roof stunts (int)
	parachute to roof stunts (int)
	total game time (int)
	rioja settlements (int)
	guerrilla settlements (int)
	unknown (int)

// general deserializer

for i = 0, 21 do
	mission i completed (byte)

	// unknown deserializer

	unknown (int)





```
