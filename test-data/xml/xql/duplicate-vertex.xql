let $failed := 
    for $e in /cut//edge
    where ($e/vertex[1]/@key = $e/vertex[2]/@key)
    return concat("//edge[@id=", $e/@id, "]")
return if (count($failed) = 0)
       then 0
       else $failed


