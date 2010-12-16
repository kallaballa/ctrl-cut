for $edge in /cut/edges/*
    let $start := $edge/vertex[1]
    let $end := $edge/vertex[2]
    let $startcnt := count(/cut/vertices/vertex[@id = $start/@id])
    let $endcnt := count(/cut/vertices/vertex[@id = $end/@id])
    return if ($startcnt = 1 and $endcnt = 1)
    then 0
    else 1
    
