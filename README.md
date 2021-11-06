#### programming language using llvmの説明

* llvmを用いた自作言語を作成しました 
* コンパイラ言語ですがインタプリタ型言語のように記述できる言語です

###### 主な特徴

1. 条件分岐
2. 条件式の結合（結合力 : and > or, 条件式の優先度：[]で囲む）
3. 繰り返し構文
4. 入出力
5. マイナス（例 : -4 , -i, --i -> i）
6. mian関数はべた書き（pythonのような）
7. 余り演算
8. 割り切り算
9. コメント (例 : # コメント)
10. +=, -=, *=, /=, //=, %= で式の省略可
11. break (例 : break(n) -> n個繰り返し構文を抜ける)
12. continue (例 : continue(n) -> n個外の繰り返し構文のbodyのはじめに飛ぶ)
13. 再帰
14. global変数宣言 (main関数以外の関数でmain関数の変数のみ宣言可能)
15. string型などの数字以外の型はサポート外
16. 変数宣言は不必要
17. 日本語で出力されるエラー文
18. 関数呼び出し以降に関数定義がされていても問題なし
19. main関数の変数は初期値が0に設定されている
20. 出力は少数か整数か判断される (例 : print(4/2, 5/2) -> 2 2.50000)
