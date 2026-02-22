{-# LANGUAGE CApiFFI #-}
foreign import capi "adder.h add"
  c_add :: Int -> Int -> Int

main :: IO ()
main = do
  let sum = c_add 2 2
  putStrLn $ show sum
