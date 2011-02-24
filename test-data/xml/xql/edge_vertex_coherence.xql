let $failed := 
    for $e in /cut//edge
        for $v in $e/vertex
        where (count(//vertices/vertex[@id = $v/@id]) != 1)
        return concat("//edge[@id=", $e/@id, "]/vertex[@id=", $v/@id, "]")
return if (count($failed) = 0)
       then 0
       else $failed


