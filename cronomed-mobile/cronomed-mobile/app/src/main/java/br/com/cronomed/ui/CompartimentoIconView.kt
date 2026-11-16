package br.com.cronomed.ui

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.graphics.Path
import android.graphics.RectF
import android.graphics.Typeface
import android.util.AttributeSet
import android.view.View
import androidx.core.content.ContextCompat
import br.com.cronomed.R

/**
 * Selo oficial do compartimento (Cap. 4 - RF13, RF16, RF17):
 * Cor fixa + Padrao grafico (listras, pontos, xadrez, zigue-zague) + Numero em circulo branco.
 */
class CompartimentoIconView @JvmOverloads constructor(
    contexto: Context,
    atributos: AttributeSet? = null,
    estilo: Int = 0
) : View(contexto, atributos, estilo) {

    private var numero = 1
    private val pincelFundo = Paint(Paint.ANTI_ALIAS_FLAG)
    private val pincelPadrao = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = Color.argb(130, 255, 255, 255)
        style = Paint.Style.STROKE
        strokeWidth = 4f
    }
    private val pincelCirculo = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = Color.WHITE
        style = Paint.Style.FILL
    }
    private val pincelBorda = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = ContextCompat.getColor(contexto, R.color.preto)
        style = Paint.Style.STROKE
        strokeWidth = 3f
    }
    private val pincelTexto = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = ContextCompat.getColor(contexto, R.color.preto)
        typeface = Typeface.DEFAULT_BOLD
        textAlign = Paint.Align.CENTER
    }
    private val rect = RectF()
    private val path = Path()

    init {
        atualizarCor()
    }

    fun definirCompartimento(n: Int) {
        numero = n.coerceIn(1, 6)
        atualizarCor()
        invalidate()
    }

    private fun atualizarCor() {
        val cor = when (numero) {
            1 -> R.color.comp_1
            2 -> R.color.comp_2
            3 -> R.color.comp_3
            4 -> R.color.comp_4
            5 -> R.color.comp_5
            else -> R.color.comp_6
        }
        pincelFundo.color = ContextCompat.getColor(context, cor)
    }

    override fun onMeasure(larguraSpec: Int, alturaSpec: Int) {
        val padrao = (44 * resources.displayMetrics.density).toInt()
        setMeasuredDimension(resolveSize(padrao, larguraSpec), resolveSize(padrao, alturaSpec))
    }

    override fun onDraw(canvas: Canvas) {
        val lado = minOf(width, height).toFloat()
        val pad = lado * 0.06f
        rect.set(pad, pad, width - pad, height - pad)
        val canto = lado * 0.22f

        canvas.drawRoundRect(rect, canto, canto, pincelFundo)
        desenharPadrao(canvas, lado)
        canvas.drawRoundRect(rect, canto, canto, pincelBorda)

        val cx = width / 2f
        val cy = height / 2f
        val raioCirculo = lado * 0.26f
        canvas.drawCircle(cx, cy, raioCirculo, pincelCirculo)
        canvas.drawCircle(cx, cy, raioCirculo, pincelBorda)

        pincelTexto.textSize = lado * 0.34f
        val base = cy - (pincelTexto.descent() + pincelTexto.ascent()) / 2f
        canvas.drawText(numero.toString(), cx, base, pincelTexto)
    }

    private fun desenharPadrao(canvas: Canvas, lado: Float) {
        val passo = lado / 5f
        when (numero) {
            1 -> { // Listras diagonais
                var i = -lado
                while (i < lado * 2) {
                    canvas.drawLine(i, 0f, i + lado, lado, pincelPadrao)
                    i += passo
                }
            }
            2 -> { // Pontos
                pincelPadrao.style = Paint.Style.FILL
                var x = passo
                while (x < width - passo / 2) {
                    var y = passo
                    while (y < height - passo / 2) {
                        canvas.drawCircle(x, y, lado * 0.04f, pincelPadrao)
                        y += passo
                    }
                    x += passo
                }
                pincelPadrao.style = Paint.Style.STROKE
            }
            3 -> { // Listras horizontais
                var y = passo
                while (y < height) {
                    canvas.drawLine(rect.left, y, rect.right, y, pincelPadrao)
                    y += passo
                }
            }
            4 -> { // Xadrez
                var p = passo
                while (p < lado) {
                    canvas.drawLine(rect.left, p, rect.right, p, pincelPadrao)
                    canvas.drawLine(p, rect.top, p, rect.bottom, pincelPadrao)
                    p += passo
                }
            }
            5 -> { // Listras verticais
                var x = passo
                while (x < width) {
                    canvas.drawLine(x, rect.top, x, rect.bottom, pincelPadrao)
                    x += passo
                }
            }
            else -> { // Zigue-zague
                path.reset()
                var y = passo
                while (y < height) {
                    path.moveTo(rect.left, y)
                    var x = rect.left
                    var up = true
                    while (x < rect.right) {
                        x += passo / 2f
                        path.lineTo(x, if (up) y - passo * 0.3f else y + passo * 0.3f)
                        up = !up
                    }
                    y += passo
                }
                canvas.drawPath(path, pincelPadrao)
            }
        }
    }
}