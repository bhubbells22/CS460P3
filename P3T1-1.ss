(define (add l1 l2)
  (if (and (not (null? l1)) (not (null? l2)) )
      (if (and (list? l1) (list? l2))
	  (cons (car l1) (cdr l2))
	  'add_requires_not_null_lists
	  )
      'add_requires_not_null_lists
      )
  )

(define (multiply num1 num2)
  (if (or (zero? num1) (zero? num2))
      'multiply_requires_non_zero_numbers
      (* num1 num2)
      )
  )

(define (main)
  (display (add '(1 2 3 4 5) '(a b c d e)) ) (newline)
  (display (multiply 21 13)) (newline)
  )

(main)
  
