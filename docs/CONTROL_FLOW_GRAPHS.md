# Управляющие графы программы

Вершины с ромбом обозначают условия, прямоугольники — операторы или возврат результата. Метки `T` и `F` соответствуют истинной и ложной ветвям.

## `applyDiscount`

```mermaid
flowchart TD
    A([Вход]) --> B{total < 0}
    B -- T --> C[return -1.0]
    B -- F --> D{total >= 10000}
    D -- T --> E{isPremium}
    E -- T --> F[return total * 0.80]
    E -- F --> G[return total * 0.85]
    D -- F --> H{total >= 5000}
    H -- T --> I{isPremium}
    I -- T --> J[return total * 0.90]
    I -- F --> K[return total * 0.95]
    H -- F --> L[return total]
```

## `calcShipping`

```mermaid
flowchart TD
    A([Вход]) --> B{total < 0}
    B -- T --> C[return -1.0]
    B -- F --> D{total >= 5000}
    D -- T --> E[return 0.0]
    D -- F --> F[return 300.0]
```

## `finalPrice`

```mermaid
flowchart TD
    A([Вход]) --> B{total < 0}
    B -- T --> C[return -1.0]
    B -- F --> D{discount < 0}
    D -- T --> C
    D -- F --> E{shipping < 0}
    E -- T --> C
    E -- F --> F[result = total - discount + shipping]
    F --> G{result < 0}
    G -- T --> H[result = 0.0]
    G -- F --> I[round и return]
    H --> I
```

## `countExpensiveItems`

```mermaid
flowchart TD
    A([Вход]) --> B[count = 0]
    B --> C{остался элемент?}
    C -- F --> G[return count]
    C -- T --> D{price > threshold}
    D -- T --> E[++count]
    D -- F --> F[следующий элемент]
    E --> F
    F --> C
```

## `reversePrices` — вариант №4

```mermaid
flowchart TD
    A([Вход]) --> B{prices.empty}
    B -- T --> C[return]
    B -- F --> D[left = 0]
    D --> E[right = size - 1]
    E --> F{left < right}
    F -- F --> K([Выход])
    F -- T --> G[swap prices left/right]
    G --> H[++left]
    H --> I[--right]
    I --> F
```

## `minOnAndAboveSecondaryDiagonal` — вариант №4

```mermaid
flowchart TD
    A([Вход]) --> B{matrix.empty}
    B -- T --> C[return 0.0]
    B -- F --> D[size = matrix.size; minimum = 0; found = false]
    D --> E{row < size}
    E -- F --> N{found}
    E -- T --> F[column = 0]
    F --> G{column < row.size}
    G -- F --> M[++row]
    G -- T --> H{row + column <= size - 1}
    H -- F --> L[++column]
    H -- T --> I{!found или value < minimum}
    I -- T --> J[minimum = value]
    J --> K[found = true]
    K --> L
    I -- F --> L
    L --> G
    M --> E
    N -- T --> O[return minimum]
    N -- F --> P[return 0.0]
```
