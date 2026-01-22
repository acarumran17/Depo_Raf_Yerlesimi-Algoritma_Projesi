Depo Raf Yerleşimi (Stok Yönetimi) – Algoritma Projesi

Bu proje, bir depodaki ürünlerin raflara en verimli şekilde yerleştirilmesini amaçlayan bir stok yönetimi / raf optimizasyonu uygulamasıdır.
Amaç; raf kapasitesini aşmadan ürünleri yerleştirip maksimum verim (efficiency) elde etmek ve farklı algoritmaların performanslarını karşılaştırmaktır.

- Projenin Amacı

Depolarda ürün yerleştirme işlemi, hem alan kullanımını hem de operasyonel verimliliği doğrudan etkiler.
Bu projede ürünler belirli bir kapasiteye sahip raflara yerleştirilirken:

Rafların daha dolu / daha verimli kullanılması

Ürünlerin en uygun kombinasyonlarla seçilmesi

Algoritmaların zaman performansı (ms, µs) açısından ölçülmesi

hedeflenmiştir.

- Projede Kullanılan Yaklaşımlar

Projede raf yerleşimi için farklı algoritma mantıkları uygulanmıştır:

1) Greedy (Açgözlü) Yaklaşım

Hızlı sonuç üretir.

Her adımda “en iyi görünen” seçimi yapar.

Her zaman optimal sonuç garantisi vermez.

2) Dynamic Programming (0/1 Knapsack)

Kapasiteyi aşmadan maksimum değer/verim sağlayan kombinasyonu bulmaya çalışır.

Optimal çözüme daha yakındır.

Greedy’ye göre daha maliyetli olabilir ancak daha doğru sonuç verir.

3) Arama / Kontrol Mekanizmaları

Raflar ve ürünler üzerinde arama işlemleri yapılabilir.

Sonuçlar found, index gibi bilgilerle döndürülür.
