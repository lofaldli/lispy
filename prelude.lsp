;
; Atoms
;
(def {nil} {})
(def {true} 1)
(def {false} 0)

;
; building blocks
;

; function definitions
(def {fun} (\ {f b} {
    def (head f) (\ (tail f) b)
}))

; unpack list for function
(fun {unpack f l} {
    eval (join (list f) l)
})

; pack list for function
(fun {pack f & xs} {f xs})

; curried and uncurried calling
(def {curry} unpack)
(def {uncurry} pack)

; perform several things in sequence
(fun {do & l} {
    if (== l nil)
        {nil}
        {last l}
})

; open new scope
(fun {let b} {
    ((\ {_} b) ())
})

;
; logical functions
;
(fun {not x}   {- 1 x})
(fun {or x y}  {+ x y})
(fun {and x y} {* x y})

;
; misc
;

(fun {flip f a b} {f b a})
(fun {ghost & xs} {eval xs})
(fun {comp f g x} {f (g x)})

;
; list functions
;

; first, second, third
(fun {fst l} {eval (head l) })
(fun {snd l} {eval (head (tail l)) })
(fun {trd l} {eval (head (tail (tail l))) })

; nth
(fun {nth n l} {
    if (== n 0)
        {fst l}
        {nth (- n 1) (tail l)}
})

; list length
(fun {len l} {
    if (== l nil)
        {0}
        {+ 1 (len (tail l))}
})

; last item
(fun {last l} {nth (- (len l) 1) l})

; take n items
(fun {take n l} {
    if (== n 0)
        {nil}
        {join (head l) (take (- n 1) (tail l))}
})

; drop n items
(fun {drop n l} {
    if (== n 0)
        {n}
        {drop (- n 1) (tail l)}
})

; split at n
(fun {split n l} {list (take n l) (drop n l)})

; element of list
(fun {elem x l} {
    if (== l nil)
        {false}
        {if (== x (fst l)) {true} {elem x (tail)}}
})

; apply function to list
(fun {map f l} {
    if (== l nil) 
        {nil}
        {join (list (f (fst l))) (map f (tail l))}
}) 

; apply filter to list
(fun {filter f l} {
    if (== l nil)
        {nil}
        {join (if (f (fst l)) {head l} {nil}) (filter f (tail l))}
})

; fold left
(fun {foldl f z l} {
    if (== l nil)
        {z}
        {foldl f (f < (fst l)) (tail l)}
 })

(fun {sum l} {foldl + 0 l})
(fun {prod l} {foldl * 1 l})

;
; conditionals
;

(fun {select & cs} {
    if (== cs nil)
        {error "no selection found"}
        {if (fst (fst cs)) {snd (fst cs)} {unpack select (tail cs)}}
 })
(def {otherwise} true)

; day of month suffix 
(fun {month-day-suffix i} {
    select
        {(== i 0 "st")}
        {(== i 1 "nd")}
        {(== i 2 "rd")}
        {otherwise "th"}
})
