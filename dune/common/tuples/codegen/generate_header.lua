dofile( "modifiers_include.lua" )

print( "// @File auto-generated by " .. arg[0] .. ", do not modify" )
print( "// @Date " .. os.date("%x", os.time()) )

local tuple_length = 8
indent = "  "

header_guard = "DUNE_COMMON_TUPLES_MODIFIERS_INCLUDE_HH"

print( "#ifndef " .. header_guard )
print( "#define " .. header_guard )

print ""

print( "#include <dune/common/tuples/tuples.hh>" )

print ""

print ( "namespace Dune" )
print ( "{" )

print ""

print( indent .. "// tuple_push_back" )
print( indent .. "// ---------------" )

print ""

for i = 1, tuple_length do
  body = tuple_push_back( i, indent )
  print( body )
  print ""
end

print ""
print ""

print( indent .. "// tuple_push_front" )
print( indent .. "// ----------------" )

print ""

for i = 1, tuple_length do
  body = tuple_push_front( i, indent )
  print( body )
  print ""
end

print ""
print ""

print( indent .. "// tuple_pop_back" )
print( indent .. "// --------------" )

print ""

for i = 1, tuple_length do
  body = tuple_pop_back( i, indent )
  print( body )
  print ""
end

print ""
print ""

print( indent .. "// tuple_pop_front" )
print( indent .. "// ---------------" )

print ""

for i = 1, tuple_length do
  body = tuple_pop_front( i, indent )
  print( body )
  print ""
end

print ( "} // namespace Dune" )

print ""

print( "#endif // #ifndef " .. header_guard )